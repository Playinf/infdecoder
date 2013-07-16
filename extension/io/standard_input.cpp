/* standard_input.cpp */
#include <istream>

/*
 * extract one sentence, append <s> and </s>, then output to parser
 */
int get_raw_input(void* args)
{
    void** argv = (void **) args;
    /* first argument: input stream */
    std::istream* input = reinterpret_cast<std::istream*>(argv[0]);
    /* second argument: output string */
    std::string* str = reinterpret_cast<std::string*>(argv[1]);

    while (!input->eof()) {

    }
}
