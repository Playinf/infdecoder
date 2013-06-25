/* main.cpp */
#include <cstdio>
#include <fstream>
#include <iostream>
#include <utility.h>
#include <rule_tree.h>

void insert_rule(rule_tree& rule_table, std::vector<std::string>& source_rule,
    std::vector<std::string>& target_rule)
{
    const rt_node* root = rule_table.get_root();
    rt_node* p = const_cast<rt_node*>(root);
    unsigned int src_size = source_rule.size();

    for (unsigned int i = 2; i < src_size; i++) {
        std::string& name = source_rule[i];
        unsigned int id = 0;
        bool terminal = true;
        const symbol* sym;
        if (name[0] == '#') {
            unsigned int pos = name.find('_');
            unsigned int end = name.length();
            
            if (pos != name.npos) {
                std::string id_str = name.substr(pos + 1, end - pos - 2);
                id = std::stoi(id_str);
            } else
                pos = end - 1;

            name = name.substr(1, pos - 1);
            terminal = false;
        }

        if (terminal)
            sym = rule_table.search_symbol(name);
        else
            sym = rule_table.search_symbol(name, id);

        p = rule_table.insert_node(p, sym);
    }
}

void load_rule_table(std::ifstream& file, rule_tree& rule_table)
{
    int line_count = 0;
    std::string line;
    std::vector<std::string> vec;
    std::vector<std::string> source_rule;
    std::vector<std::string> target_rule;

    while (std::getline(file, line)) {
        line_count++;
        
        vec.clear();
        string_split(line, "|||", vec);

        if (vec.size() != 4) {
            std::cerr << "line: " << line_count << " \n";
            std::cerr << "error in loading rule table\n";
            std::exit(-1);
        }

        std::string& source_rule_str = vec[0];
        std::string& target_rule_str = vec[1];
        std::string& rule_feature_str = vec[2];

        source_rule.clear();
        source_rule.clear();

        string_split(source_rule_str, " ", source_rule);
        string_split(target_rule_str, " ", target_rule);
        insert_rule(rule_table, source_rule, target_rule);
    }
}

int main()
{
    std::ifstream file("rules.txt");
    rule_tree tree;

    if (!file) {
        std::cerr << "cannot open file `rules'\n";
        std::exit(-1);
    }

    load_rule_table(file, tree);

    std::cout << "OK\n";
}
