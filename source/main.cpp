/* main.cpp */
#include <iostream>

/* TODO:
 * version 0.1
 * 1. add nbest support (finished)
 * 2. add multi-thread support (finished)
 * 3. add dynamic linked feature function
 * 4. add dynamic linked io function
 */

 void print_help()
{
    std::cout << "infdecoder version 0.1" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t-c | --config \t configuration file name" << std::endl;
    std::cout << "\t-h | --help \t show this help" << std::endl;
    std::cout << "\t-v | --version \t show version" << std::endl;
    std::cout << std::endl;
    std::cout << "if there are any bugs, ";
    std::cout << "please contact playinf@stu.xmu.edu.cn" << std::endl;
}

void print_version()
{
    std::cout << "infdecoder version 0.1" << std::endl;
    std::cout << std::endl;
    std::cout << "if there are any bugs, ";
    std::cout << "please contact playinf@stu.xmu.edu.cn" << std::endl;
}

/* moses extensions */
void moses_translate(int argc, char** argv);

int main(int argc, char** argv)
{
    moses_translate(argc, argv);
}
