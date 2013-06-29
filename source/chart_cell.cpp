/* chart_cell.cpp */
#include <string>
#include <beam.h>
#include <cube.h>
#include <symbol.h>
#include <rule_set.h>
#include <chart_cell.h>
#include <hypothesis.h>

chart_cell::chart_cell()
{
    x_symbol_beam = nullptr;
    s_symbol_beam = nullptr;
    symbol_set.reserve(2);
}

chart_cell::~chart_cell()
{
    if (x_symbol_beam) {
        delete x_symbol_beam;
    }

    if (s_symbol_beam) {
        delete s_symbol_beam;
    }
}

void chart_cell::add_hypothesis(hypothesis* hypo)
{
    const symbol* start_symbol = hypo->get_start_symbol();
    const std::string& name = *start_symbol->get_name();
    static auto& x_beam_size_str = parameter::get_parameter("x-beam-size");
    static auto& s_beam_size_str = parameter::get_parameter("x-beam-size");
    static auto& x_thres_str = parameter::get_parameter("x-beam-threshold");
    static auto& s_thres_str = parameter::get_parameter("s-beam-threshold");
    static unsigned int x_beam_size = std::atoi(x_beam_size_str.c_str());
    static unsigned int s_beam_size = std::atoi(s_beam_size_str.c_str());
    static double x_beam_threshold = std::atof(x_thres_str.c_str());
    static double s_beam_threshold = std::atof(s_thres_str.c_str());

    if (name == "X") {
        if (!x_symbol_beam) {
            symbol_set.push_back(start_symbol);
            x_symbol_beam = new beam(x_beam_size, x_beam_threshold);
        }

        x_symbol_beam->insert_hypothesis(hypo);
    } else if (name == "S") {
        if (!s_symbol_beam) {
            symbol_set.push_back(start_symbol);
            s_symbol_beam = new beam(s_beam_size, s_beam_threshold);
        }

        s_symbol_beam->insert_hypothesis(hypo);
    }

    return;
}

const std::vector<const symbol*>& chart_cell::get_symbol_set() const
{
    return symbol_set;
}

chart_cell::hypothesis_list* chart_cell::get_hypothesis_list(const symbol* lhs)
{
    const std::string& name = *lhs->get_name();

    if (lhs->is_terminal())
        return nullptr;

    if (name == "X") {

        if (x_symbol_beam == nullptr)
            return nullptr;

        return x_symbol_beam->get_sorted_hypothesis_list();
    } else if (name == "S") {

        if (s_symbol_beam == nullptr)
            return nullptr;

        return s_symbol_beam->get_sorted_hypothesis_list();
    }

    return nullptr;
}

void chart_cell::decode(rule_set* s)
{
    unsigned int size = s->size();
    cube_queue queue;
    static const std::string* lim_str = &parameter::get_parameter("pop-limit");
    static unsigned int limit = std::atoi(lim_str->c_str());

    for (unsigned int i = 0; i < size; i++) {
        rule_list* list = s->get_rule_list(i);
        //list->print();
        cube* rule_cube = new cube(list);

        queue.insert(rule_cube);
    }

    for (unsigned int i = 0; i < limit; i++) {
        if (queue.empty())
            break;
        hypothesis* hypo = const_cast<hypothesis*>(queue.pop());

        add_hypothesis(hypo);
    }
}

void chart_cell::sort()
{
    if (x_symbol_beam)
        x_symbol_beam->sort();

    if (s_symbol_beam)
        s_symbol_beam->sort();
}

#include <verbose.h>

void chart_cell::print()
{
    if (x_symbol_beam)
        print_beam(x_symbol_beam);

    if (s_symbol_beam)
        print_beam(s_symbol_beam);
}
