/*
 * jm_state.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __JM_STATE_H__
#define __JM_STATE_H__

#include <state.h>
#include <lm_state.h>

#define override ;

class jm_state : public state {
public:
    jm_state(const hypothesis* hypo, unsigned int id);
    ~jm_state();

    jm_state& operator=(const jm_state&) = delete;

    float get_score() const;
    const std::vector<unsigned int>* get_affiliation() const;
    const std::vector<const std::string*>* get_prefix() const;
    const std::vector<const std::string*>* get_suffix() const;
    const std::vector<const std::string*>* get_translation() const;

    void set_score(float score);
    void set_affiliation(std::vector<unsigned int>& vec);
    void set_translation(std::vector<const std::string*>& vec);
    int compare(const state* jmstate) const override;
    int compare(const jm_state* state) const;
    void calculate_prefix_suffix(unsigned int order);
private:
    float score;
    unsigned int id;
    unsigned int affiliation_id;
    const hypothesis* state_hypothesis;
    std::vector<unsigned int> affiliation;
    std::vector<const std::string*> prefix;
    std::vector<const std::string*> suffix;
    std::vector<const std::string*> translation;
};

#endif /* __JM_STATE_H__ */
