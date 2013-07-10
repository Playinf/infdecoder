/* parser.cpp */
#include <queue>
#include <memory>
#include <string>
#include <cstdlib>
#include <unordered_set>
#include <config.h>
#include <parser.h>
#include <feature.h>
#include <trellis.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <rule_finder.h>

#include <iostream>
#include <verbose.h>

typedef std::priority_queue<trellis_detour*, std::vector<trellis_detour*>,
    trellis_detour_less> queue_type;
typedef std::vector<const std::string*> output_type;

struct output_hash {
    size_t operator()(const output_type& o) const
    {
        auto size = o.size();
        std::hash<const std::string*> hasher;
        size_t val = 0;

        for (unsigned int i = 0; i < size; i++) {
            const std::string* str = o[i];
            val += hasher(str);
        }

        return val;
    }
};

struct output_equal {
    bool operator()(const output_type& o1, const output_type& o2) const
    {
        auto size1 = o1.size();
        auto size2 = o2.size();
        auto i = size1;

        if (size1 != size2)
            return false;

        for (i = 0; i < size1; i++) {
            const std::string* s1 = o1[i];
            const std::string* s2 = o2[i];

            if (s1 != s2)
                return false;
        }

        return true;
    }
};

static void lazy_next(std::shared_ptr<const trellis_path> basepath,
    const trellis_node* node, queue_type* queue)
{
    const hypothesis* hypo = node->get_hypothesis();
    const hypothesis* recombined_hypo = hypo->get_recombined_hypothesis();

    if (recombined_hypo != nullptr) {
        queue->push(new trellis_detour(basepath, node, recombined_hypo));
    }

    unsigned int child_num = node->get_children_num();

    for (unsigned int i = 0; i < child_num; i++) {
        const trellis_node* n = node->get_child(i);
        lazy_next(basepath, n, queue);
    }
}

parser::parser(std::vector<rule_tree*>* tree_vec)
{
    input = nullptr;
    table = nullptr;
    applicable_rule_set = nullptr;
    tree_vector = tree_vec;

    unsigned int size = tree_vec->size();

    for (unsigned int i = 0; i < size; i++) {
        rule_tree* tree = tree_vector->at(i);
        rule_finder* finder = new rule_finder(tree);
        rule_lookup_manager.push_back(finder);
    }
    /*
    std::string span_limit = parameter::get_parameter("span-limit");
    unsigned int limit = std::atoi(span_limit.c_str());
    rule_lookup_manager[0]->set_span_limit(limit);
    rule_lookup_manager[1]->set_span_limit(0);
    */
}

parser::~parser()
{
    clear();

    unsigned int size = rule_lookup_manager.size();

    for (unsigned int i = 0; i < size; i++)
        delete rule_lookup_manager[i];
}

void parser::parser_initialize()
{
    unsigned int size = input->size();
    unsigned int n = rule_lookup_manager.size();

    clear();
    table = new chart(size);
    applicable_rule_set = new rule_set();

    for (unsigned int i = 0; i < n; i++) {
        rule_finder* finder = rule_lookup_manager[i];
        finder->initialize(input, table);
    }
}

void parser::clear()
{
    unsigned int size = rule_lookup_manager.size();

    for (unsigned int i = 0; i < size; i++) {
        rule_finder* finder = rule_lookup_manager[i];
        finder->clear();
    }

    if (table) {
        delete table;
        table = nullptr;
    }

    if (applicable_rule_set) {
        delete applicable_rule_set;
        applicable_rule_set = nullptr;
    }
}

void parser::parse(input_type& input)
{
    size_type n = input.size();
    size_type size = rule_lookup_manager.size();
    chart_cell* cell;
    unsigned int pop_limit = 100;

    this->input = &input;
    parser_initialize();

    for (size_type len = 1; len <= n; len++) {

        for (size_type i = 0; i <= n - len; i++) {
            size_type j = i + len - 1;

            //std::cout << "[" << i << ", " << j << "]" << std::endl;

            cell = table->get_cell(i, j);
            applicable_rule_set->clear();

            for (unsigned int k = 0; k < size; k++) {
                rule_finder* finder = rule_lookup_manager[k];
                /*
                unsigned int span_limit = finder->get_span_limit();

                if (span_limit && len > span_limit)
                    continue;

                std::cout << "rule finder " << k << std::endl;
                */
                finder->find(i, j, *applicable_rule_set);

            }
            //print_rule_set(applicable_rule_set);
            cell->decode(applicable_rule_set, pop_limit);
            cell->sort();
            //print_chart_cell(cell);
        }
    }
}

void parser::get_all_hypothesis(std::vector<hypothesis*>& hypo_vec)
{
    chart_cell* cell = table->get_cell(0, input->size() - 1);

    cell->get_all_hypothesis(&hypo_vec);
}

void parser::get_nbest(unsigned int num, path_vector* path_list, bool distinct)
{
    std::vector<hypothesis*> hypothesis_list;
    unsigned int count = 0;

    if (num == 0)
        return;

    chart_cell* cell = table->get_cell(0, input->size() - 1);
    cell->get_all_hypothesis(&hypothesis_list);
    unsigned int size = hypothesis_list.size();

    /* no hypothesis */
    if (size == 0)
        return;

    hypothesis* hypo = hypothesis_list[0];
    std::shared_ptr<trellis_path> path(new trellis_path(hypo));

    if (num == 1) {
        path_list->push_back(path);
        return;
    }

    queue_type detour_queue;

    for (unsigned int i = 0; i < size; i++) {
        hypothesis* h = hypothesis_list[i];
        std::shared_ptr<trellis_path> path(new trellis_path(h));
        detour_queue.push(new trellis_detour(path, path->get_final_node(), h));
    }

    std::unordered_set<output_type, output_hash, output_equal> output_set;

    while (!detour_queue.empty() && count < num) {
        const trellis_detour* detour = detour_queue.top();
        std::shared_ptr<trellis_path> path(new trellis_path(detour));

        detour_queue.pop();

        if (!distinct) {
            path_list->push_back(path);
            count++;
        } else {
            std::vector<const std::string*> output;
            path->output(&output);
            if (output_set.insert(output).second) {
                path_list->push_back(path);
                count++;
            }
        }

        lazy_next(path, path->get_deviation_node(), &detour_queue);
        delete detour;
    }

    while (!detour_queue.empty()) {
        const trellis_detour* detour = detour_queue.top();
        detour_queue.pop();
        delete detour;
    }
}
