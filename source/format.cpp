/* format.cpp */
#include <string>
#include <fstream>
#include <iostream>
#include <model.h>
#include <format.h>
#include <parser.h>
#include <utility.h>
#include <rule_tree.h>

inline bool is_nonterminal_string(std::string& str)
{
    std::string::size_type end = str.length() - 1;

    if (str[0] == '#' && str[end] == '#')
        return true;

    return false;
}

void parse_nonterminal_string(std::string& str, std::string& name, int& id)
{
    std::string::size_type pos = str.find('_');
    std::string::size_type end = str.length();
    
    if (pos != name.npos) {
        std::string id_str = str.substr(pos + 1, end - pos - 2);
        id = std::atoi(id_str.c_str());
    } else
        pos = end - 1;
    
    name = str.substr(1, pos - 1);
}

void parse_rule_feature(std::string& str, std::vector<double>& feature)
{
    std::vector<std::string> feature_str;
    unsigned int size;

    string_split(str, " ", feature_str);
    size = feature_str.size();

    for (unsigned int i = 0; i < size; i++)
        feature.push_back(std::atof(feature_str[i].c_str()));
}

void insert_rule(rule_tree& rule_table, std::vector<std::string>& source_rule,
    std::vector<std::string>& target_rule, std::vector<double>& prob)
{
    const rt_node* root = rule_table.get_root();
    rt_node* p = const_cast<rt_node*>(root);
    unsigned int src_size = source_rule.size();
    unsigned int tgt_size = target_rule.size();
    unsigned int tgt_indx = 2;
    bool reorder = false;
    unsigned tgt_symbol_num = tgt_size - 2;
    std::string start_symbol_string;
    const symbol* start_symbol;
    std::string trans_des = "translation probablity";
    std::string rev_trans_des = "reverse translation probablity";
    std::string lex_des = "lexical weight";
    std::string rev_lex_des = "reverse lexical weight";
    std::string word_des = "word_penalty";
    unsigned int trans_id = model::get_feature_id(trans_des);
    unsigned int rev_trans_id = model::get_feature_id(rev_trans_des);
    unsigned int lex_id = model::get_feature_id(lex_des);
    unsigned int rev_lex_id = model::get_feature_id(rev_lex_des);
    unsigned int word_id = model::get_feature_id(word_des);
    feature::feature_function func = translation_feature_function;
    int id;

    parse_nonterminal_string(source_rule[0], start_symbol_string, id);
    start_symbol = symbol_table::search_symbol(start_symbol_string, false);
    rule* r = new rule(tgt_symbol_num);
    r->set_start_symbol(start_symbol);

    for (unsigned int i = 2; i < src_size; i++) {
        std::string& src_name = source_rule[i];
        bool src_terminal = true;
        bool tgt_terminal = true;
        int src_id = 0;
        int tgt_id = 0;
        const symbol* src_sym;
        const symbol* tgt_sym = nullptr;
        
        if (is_nonterminal_string(src_name)) {
            parse_nonterminal_string(src_name, src_name, src_id);
            src_terminal = false;

            for (unsigned int j = tgt_indx; j < tgt_size; j++) {
                std::string& tgt_name = target_rule[j];

                if (is_nonterminal_string(tgt_name)) {
                    parse_nonterminal_string(tgt_name, tgt_name, tgt_id);
                    tgt_terminal = false;

                    if (tgt_id != src_id)
                        reorder = true;
                }

                tgt_sym = symbol_table::search_symbol(tgt_name, tgt_terminal);
                r->add_symbol(tgt_sym);
                tgt_indx++;

                if (!tgt_terminal)
                    break;
            }
        }
        src_sym = symbol_table::search_symbol(src_name, src_terminal);

        p = rule_table.insert_node(p, src_sym);
    }

    for (unsigned int i = tgt_indx; i < tgt_size; i++) {
        const symbol* sym = symbol_table::search_symbol(target_rule[i]);
        r->add_symbol(sym);
    }

    r->set_order(reorder);
    feature tmp_feature = feature(trans_id, func, prob[0]);
    r->add_feature(&tmp_feature);
    tmp_feature = feature(rev_trans_id, func, prob[1]);
    r->add_feature(&tmp_feature);
    tmp_feature = feature(lex_id, func, prob[2]);
    r->add_feature(&tmp_feature);
    tmp_feature = feature(rev_lex_id, func, prob[3]);
    r->add_feature(&tmp_feature);

    rule::push_weight(model::get_weight(trans_id));
    rule::push_weight(model::get_weight(rev_trans_id));
    rule::push_weight(model::get_weight(lex_id));
    rule::push_weight(model::get_weight(rev_lex_id));
    rule::push_weight(model::get_weight(word_id));

    r->evaluate_score();

    rule_table.insert_rule(p, r);
}

void load_rule_table(void* args)
{
    void** argv = (void**) args;
    int line_count = 0;
    std::string line;
    std::vector<std::string> vec;
    std::vector<std::string> source_rule;
    std::vector<std::string> target_rule;
    std::vector<double> rule_feature;
    std::ifstream& file = *(std::ifstream*) argv[0];
    rule_tree& rule_table = *(rule_tree*) argv[1];

    while (std::getline(file, line)) {
        line_count++;
        
        vec.clear();
        string_split(line, "|||", vec);

        std::string& source_rule_str = vec[0];
        std::string& target_rule_str = vec[1];
        std::string& rule_feature_str = vec[3];

        source_rule.clear();
        target_rule.clear();
        rule_feature.clear();

        string_split(source_rule_str, " ", source_rule);
        string_split(target_rule_str, " ", target_rule);

        parse_rule_feature(rule_feature_str, rule_feature);
        insert_rule(rule_table, source_rule, target_rule, rule_feature);

        if (line_count && (line_count % 100000 == 0))
            std::cout << line_count << std::endl;
    }
}

void str2input(std::string& s, std::vector<const symbol*>& input)
{
    std::vector<std::string> temp;
    unsigned int size;

    string_split(s, " ", temp);
    size = temp.size();
    
    for (unsigned int i = 0; i < size; i++)
        input.push_back(symbol_table::search_symbol(temp[i]));
}

void process_input_file(void* args)
{
    std::string line;
    unsigned int nbest;
    unsigned int count = 0;
    void** argv = (void**) args;
    std::vector<const symbol*> input;
    std::vector<const std::string*> output;
    std::ifstream& input_file = *(std::ifstream*) argv[0];
    std::ofstream& output_file = *(std::ofstream*) argv[1];
    parser& syntax_analyzer = *(parser*) argv[2];
    nbest = *(unsigned int*) argv[3];

    while (std::getline(input_file, line)) {
        std::string::size_type pos1 = line.find('>');
        std::string::size_type pos2 = line.rfind('<');

        if (pos2 <= pos1 || pos1 == line.npos || pos2 == line.npos) {
            output_file << line << std::endl;
            continue;
        }

        input.clear();
        std::string sentence = line.substr(pos1 + 1, pos2 - pos1 - 1);

        std::cout << "translating...\n";
        std::cout << sentence << std::endl;
        sentence = "<s> " + sentence + " </s>";

        str2input(sentence, input);
        syntax_analyzer.parse(input);
        hypothesis* hypo = syntax_analyzer.get_hypothesis(0);
        output.clear();
        hypo->output(output);

        std::string translation;
        unsigned int output_size = output.size();

        for (unsigned int j = 1; j < output_size - 1; j++)
            translation += *output[j] + " ";

        line.replace(pos1 + 2, pos2 - pos1 - 2, translation);
        output_file << line << std::endl;
        std::cout << "translation: " << std::endl;
        std::cout << translation << std::endl;
        std::cout << "lm score: " << hypo->get_feature(0)->get_score();
        std::cout << " prefix score: " << hypo->get_heuristic_score();
        std::cout << std::endl;

        /*for (unsigned int i = 0; i < nbest; i++) {
            hypothesis* hypo = syntax_analyzer.get_hypothesis(i);

            if (hypo == nullptr)
                break;

            output.clear();
            hypo->output(output);
            output_file << count << " ||| ";
            std::string translation;
            unsigned int output_size = output.size();

            for (unsigned int j = 1; j < output_size - 1; j++)
                translation += *output[j] + " ";
            output_file << translation << " ||| ";
            const feature* lm_feature = hypo->get_feature(0);
            const feature* word_feature = hypo->get_feature(1);
            output_file << "lm: " << lm_feature->get_score() << " ";
            output_file << "w: " << word_feature->get_score() << " ";
            output_file << "tm: ";

            for (unsigned int j = 2; j < 8; j++) {
                const feature* fea = hypo->get_feature(j);
                output_file << fea->get_score() << " ";
            }

            output_file << "||| " << hypo->get_total_score() << std::endl;
        }*/
        
        count++;
    }
}
