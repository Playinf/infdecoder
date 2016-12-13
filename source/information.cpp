/* information.cpp */
#include <vector>
#include <symbol.h>
#include <information.h>

information::information()
{
    sentence = nullptr;
}

information::~information()
{
    // do nothing
}

std::vector<const symbol*>* information::get_sentence() const
{
    return sentence;
}

void information::set_sentence(std::vector<const symbol*>* sentence)
{
    this->sentence = sentence;
}
