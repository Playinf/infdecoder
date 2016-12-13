#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <utility.h>

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "add_penalty in out" << std::endl;
        return -1;
    }

    std::ifstream ifile;
    std::ofstream ofile;

    ifile.open(argv[1]);
    ofile.open(argv[2]);

    if (ifile.fail()) {
        std::cerr << "cannot open " << argv[1] << std::endl;
        return -1;
    }

    if (ofile.fail()) {
        std::cerr << "cannot open " << argv[2] << std::endl;
        return -1;
    }

    std::string line;

    while (std::getline(ifile, line)) {
        std::vector<std::string> str_vec;
        std::string newline;

        string_split(line, "|||", str_vec);

        str_vec[2] += "2.718 ";

        for (unsigned int i = 0; i < str_vec.size() - 1; i++) {
            newline += str_vec[i] + "|||";
        }

        newline += str_vec[str_vec.size() - 1];

        ofile << newline << std::endl;
    }

}
