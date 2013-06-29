/* beam.cpp */
#include <queue>
#include <limits>
#include <iterator>
#include <algorithm>
#include <beam.h>

beam::beam(size_type histogram, float threshold)
{
    sorted = false;
    max_score = -1000000.0f;
    beam_size = histogram;
    this->threshold = threshold;
    ordered_hypothesis_list.reserve(beam_size);
}

beam::~beam()
{
    /* delete all hypothesis in the beam */
    auto begin = hypothesis_set.begin();
    auto end = hypothesis_set.end();
    auto iter = begin;

    while (iter != end) {
        hypothesis* hypo = *iter;
        delete hypo;
        ++iter;
    }
}

void beam::insert_hypothesis(hypothesis* hypo)
{
    float total_score = hypo->get_total_score();
    size_type size = hypothesis_set.size();

    // threshold pruning
    if (total_score < max_score + threshold) {
        delete hypo;
        return;
    } else {
        auto result = hypothesis_set.insert(hypo);

        if (result.second) {
            /* inserted */
            sorted = false;

            if (total_score > max_score)
                max_score = total_score;

            if (size + 1 >= 2 * beam_size)
                prune();
        } else {
            /* recombining hypothesis */
            hypothesis* h = *result.first;
            float s1 = h->get_total_score();
            float s2 = total_score;

            if (s1 > s2)
                h->recombine(hypo);
            else {
                hypothesis_set.erase(result.first);
                hypo->recombine(h);
                hypothesis_set.insert(hypo);
                sorted = false;
            }
        }
    }
}

std::vector<hypothesis*>* beam::get_sorted_hypothesis_list()
{
    return &ordered_hypothesis_list;
}

void beam::sort()
{
    unsigned int size = hypothesis_set.size();
    auto begin = hypothesis_set.begin();
    auto end = hypothesis_set.end();

    if (sorted)
        return;

    ordered_hypothesis_list.clear();

    if (size > beam_size) {
        std::vector<hypothesis*> vec;
        vec.reserve(size);

        std::copy(begin, end, std::back_inserter(vec));
        std::sort(vec.begin(), vec.end(), hypothesis_less());

        auto v_begin = vec.begin();
        auto v_end = v_begin + beam_size;
        std::copy(v_begin, v_end, std::back_inserter(ordered_hypothesis_list));

    } else {
        std::copy(begin, end, std::back_inserter(ordered_hypothesis_list));

        if (size <= 1)
            return;

        auto list_begin = ordered_hypothesis_list.begin();
        auto list_end = ordered_hypothesis_list.end();

        std::sort(list_begin, list_end, hypothesis_less());
    }

    sorted = true;
}

void beam::prune()
{
    unsigned int size = hypothesis_set.size();

    if (size <= beam_size)
        return;

    std::priority_queue<float> queue;
    auto iter = hypothesis_set.begin();
    auto end = hypothesis_set.end();

    while (iter != end) {
        queue.push((*iter)->get_total_score());
        ++iter;
    }

    for (unsigned int i = 1; i < beam_size; i++)
        queue.pop();

    float s = queue.top();
    bool last_found = false;
    iter = hypothesis_set.begin();

    while (iter != end) {
        hypothesis* h = *iter;
        float h_score = h->get_total_score();
        auto iter_remove = iter++;

        if (h_score <= s) {
            if (h_score == s && !last_found)
                last_found = true;
            else {
                hypothesis_set.erase(iter_remove);
                delete h;
            }
        }
    }
}
