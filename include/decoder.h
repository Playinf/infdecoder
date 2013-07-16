/*
 * decoder.h
 * decode one sentence
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __DECODER_H__
#define __DECODER_H__

#include <memory>
#include <vector>

class lexer;
class parser;
class hypothesis;
class trellis_path;

class decoder {
public:
    typedef std::vector<std::shared_ptr<const trellis_path>> path_vector;
    decoder();
    ~decoder();

    decoder(const decoder& d) = delete;
    decoder& operator=(const decoder& d) = delete;

    hypothesis* get_best_hypothesis() const;
    void get_nbest_list(path_vector& vec) const;

    void process(const std::string& sentence);
private:
    lexer* lexical_analyzer;
    parser* syntax_analyzer;
};

#endif /* __DECODER_H__ */
