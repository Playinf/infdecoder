/*
 * handler.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __HANDLER_H__
#define __HANDLER_H__

class rule;
class symbol;
class hypothesis;
class trellis_path;

typedef rule* (*unknow_word_handler)(const symbol* word);
typedef void (*hypothesis_handler)(const hypothesis* hypo, std::string& out);
typedef void (*nbest_handler)(const trellis_path* path, std::string& out);

#endif /* __HANDLER_H__ */
