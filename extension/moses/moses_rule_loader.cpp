/* moses_rule_loader.cpp */
#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <model.h>
#include <config.h>
#include <parser.h>
#include <symbol.h>
#include <utility.h>
#include <verbose.h>
#include <rule_tree.h>

inline float floor_score(float score)
{
    const float min_score = -100.0f;

    return std::max(score, min_score);
}

inline bool is_nonterminal(const std::string& str)
{
    std::string::size_type end = str.length() - 1;

    if (str[0] == '[' && str[end] == ']')
        return true;

    return false;
}

inline void parse_nonterminal(const std::string& str, std::string& lhs)
{
    lhs = str.substr(1, str.size() - 2);
}

inline void parser_nonterminal_map(const std::string& str, std::string& src,
    std::string& tgt)
{
    std::string::size_type pos = str.find(']');
    std::string::size_type s1 = 1;
    std::string::size_type e1 = pos - 1;
    std::string::size_type s2 = pos + 2;
    std::string::size_type e2 = str.size() - 2;

    src = str.substr(s1, s1 - e1 + 1);
    tgt = str.substr(s2, s2 - e2 + 1);
}

static void parse_rule_score(const std::string& str,
    std::vector<double>& feature)
{
    std::vector<std::string> feature_str;
    unsigned int size;

    string_split(str, " ", feature_str);
    size = feature_str.size();

    for (unsigned int i = 0; i < size; i++)
        feature.push_back(std::stod(feature_str[i]));
}

static void parse_alignment(std::string& str,
    std::vector<std::pair<int, int>>& align)
{
    std::vector<std::string> align_str;

    string_split(str, " ", align_str);

    unsigned int size = align_str.size();

    /* no alignment specified */
    if (!size)
        return;

    for (unsigned int i = 0; i < size; i++) {
        std::string& s = align_str[i];
        auto pos = s.find('-');
        std::string s1 = s.substr(0, pos);
        std::string s2 = s.substr(pos + 1);
        align.push_back(std::make_pair(std::stoi(s1), std::stoi(s2)));
    }
}

static void insert_rule(rule_tree& rule_table, std::vector<std::string>& src,
    std::vector<std::string>& tgt, std::vector<double>& prob,
    std::vector<std::pair<int, int>>& align)
{
    std::string src_lhs;
    std::string tgt_lhs;
    const symbol* src_lhs_symbol;
    const symbol* tgt_lhs_symbol;
    unsigned int src_size = src.size();
    unsigned int tgt_size = tgt.size();
    std::string& src_lhs_str = src[src_size - 1];
    std::string& tgt_lhs_str = tgt[tgt_size - 1];
    std::vector<unsigned int> src_nonterm_map;
    std::vector<unsigned int> tgt_nonterm_map;
    unsigned int nonterminal_number = 0;
    symbol_table* table = symbol_table::get_instance();
    const rt_node* root = rule_table.get_root();
    rt_node* p = const_cast<rt_node*>(root);
    rule* target_rule;

    parse_nonterminal(src_lhs_str, src_lhs);
    src_lhs_symbol = table->search_symbol(src_lhs, false);
    parse_nonterminal(tgt_lhs_str, tgt_lhs);
    tgt_lhs_symbol = table->search_symbol(tgt_lhs, false);

    /* insert source side rule */
    for (unsigned int i = 0; i < src_size - 1; i++) {
        std::string& token = src[i];
        std::string src_nonterminal;
        std::string tgt_nonterminal;

        if (is_nonterminal(token)) {
            const symbol* s1;
            const symbol* s2;
            parser_nonterminal_map(token, src_nonterminal, tgt_nonterminal);
            s1 = table->search_symbol(src_nonterminal, false);
            s2 = table->search_symbol(tgt_nonterminal, false);
            p = rule_table.insert_node(p, s1, s2);
            src_nonterm_map.push_back(++nonterminal_number);
        } else {
            const symbol* s = table->search_symbol(token);
            p = rule_table.insert_node(p, s);
            src_nonterm_map.push_back(0);
        }
    }

    target_rule = new rule(tgt_size - 1);
    target_rule->set_start_symbol(src_lhs_symbol, tgt_lhs_symbol);
    nonterminal_number = 0;

    /* create target side rule */
    for (unsigned int i = 0; i < tgt_size - 1; i++) {
        std::string& token = tgt[i];
        std::string src_nonterminal;
        std::string tgt_nonterminal;

        if (is_nonterminal(token)) {
            const symbol* s2;
            parser_nonterminal_map(token, src_nonterminal, tgt_nonterminal);
            s2 = table->search_symbol(tgt_nonterminal, false);
            target_rule->add_symbol(s2);
            tgt_nonterm_map.push_back(++nonterminal_number);
        } else {
            const symbol* s = table->search_symbol(token);
            target_rule->add_symbol(s);
            tgt_nonterm_map.push_back(0);
        }
    }

    /* insert scores */
    std::vector<float> score_vec;
    unsigned int prob_size = prob.size();

    for (unsigned int i = 0; i < prob_size; i++) {
        /* convert to log probability */
        float score = std::log(prob[i]);
        score_vec.push_back(floor_score(score));
    }

    target_rule->set_score(score_vec);

    unsigned int align_size = align.size();

    /* insert alignments */
    for (unsigned int i = 0; i < align_size; i++) {
        std::pair<int, int>& p = align[i];

        if (!src_nonterm_map[p.first])
            continue;

        unsigned int src_idx = src_nonterm_map[p.first] - 1;
        unsigned int tgt_idx = tgt_nonterm_map[p.second] - 1;

        target_rule->add_align(src_idx, tgt_idx);
    }

    rule_table.insert_rule(p, target_rule);
}

static void analyze_moses_rule(std::vector<std::string>& src,
    std::vector<std::string>& tgt, std::vector<double>& prob,
    std::vector<std::pair<int, int>>& align)
{
    std::string src_lhs;
    std::string tgt_lhs;
    const symbol* src_lhs_symbol;
    const symbol* tgt_lhs_symbol;
    unsigned int src_size = src.size();
    unsigned int tgt_size = tgt.size();
    std::string& src_lhs_str = src[src_size - 1];
    std::string& tgt_lhs_str = tgt[tgt_size - 1];
    symbol_table* table = symbol_table::get_instance();

    parse_nonterminal(src_lhs_str, src_lhs);
    src_lhs_symbol = table->search_symbol(src_lhs);

    std::cout << *src_lhs_symbol->get_name();
    std::cout << " -> ";

    for (unsigned int i = 0; i < src_size - 1; i++) {
        std::string& token = src[i];
        std::string src_nonterminal;
        std::string tgt_nonterminal;

        if (is_nonterminal(token)) {
            const symbol* s1;
            const symbol* s2;
            parser_nonterminal_map(token, src_nonterminal, tgt_nonterminal);
            s1 = table->search_symbol(src_nonterminal);
            s2 = table->search_symbol(tgt_nonterminal);
            std::cout << *s1->get_name();
            std::cout << "-";
            std::cout << *s2->get_name();
            std::cout << " ";
        } else {
            const symbol* s = table->search_symbol(token);
            std::cout << *s->get_name();
            std::cout << " ";
        }
    }

    std::cout << " ||| ";

    parse_nonterminal(tgt_lhs_str, tgt_lhs);
    tgt_lhs_symbol = table->search_symbol(tgt_lhs);
    std::cout << *tgt_lhs_symbol->get_name();
    std::cout << " -> ";

    for (unsigned int i = 0; i < tgt_size - 1; i++) {
        std::string& token = tgt[i];
        std::string src_nonterminal;
        std::string tgt_nonterminal;

        if (is_nonterminal(token)) {
            const symbol* s1;
            const symbol* s2;
            parser_nonterminal_map(token, src_nonterminal, tgt_nonterminal);
            s1 = table->search_symbol(src_nonterminal);
            s2 = table->search_symbol(tgt_nonterminal);
            std::cout << *s1->get_name();
            std::cout << "-";
            std::cout << *s2->get_name();
            std::cout << " ";
        } else {
            const symbol* s = table->search_symbol(token);
            std::cout << *s->get_name();
            std::cout << " ";
        }
    }

    std::cout << " ||| ";

    for (unsigned int i = 0; i < prob.size(); i++) {
        std::cout << prob[i] << " ";
    }

    std::cout << " ||| ";

    for (unsigned int i = 0; i < align.size(); i++) {
        std::pair<int, int>& p = align[i];
        std::cout << src[p.first] << "-" << tgt[p.second] << " ";
    }

    std::cout << std::endl;
}

static void moses_rule_table_loader(std::ifstream& file, rule_tree& table,
    bool output)
{
    std::string line;
    std::vector<std::string> vec;
    std::vector<std::string> source_rule;
    std::vector<std::string> target_rule;
    std::vector<double> rule_score;
    std::vector<std::pair<int, int>> align_info;
    int line_count = 0;

    while (std::getline(file, line)) {
        line_count++;

        vec.clear();
        string_split(line, "|||", vec);

        std::string& source_rule_str = vec[0];
        std::string& target_rule_str = vec[1];
        std::string& rule_score_str = vec[2];
        std::string& align_str = vec[3];

        source_rule.clear();
        target_rule.clear();
        rule_score.clear();
        align_info.clear();

        string_split(source_rule_str, " ", source_rule);
        string_split(target_rule_str, " ", target_rule);
        parse_rule_score(rule_score_str, rule_score);
        parse_alignment(align_str, align_info);
        insert_rule(table, source_rule, target_rule, rule_score, align_info);

        if (output && line_count && (line_count % 100000 == 0))
            std::cerr << line_count << std::endl;
    }
}

void moses_format_loader(void* args)
{
    void** argv = (void**) args;
    const char* file_name = (const char*) argv[0];
    rule_tree& table = *(rule_tree*) argv[1];
    std::ifstream file(file_name);

    if (file.fail()) {
        std::cerr << "FATAL ERROR: cannot open " << file_name << std::endl;
        exit(-1);
    }

    moses_rule_table_loader(file, table, true);

    file.close();
}
