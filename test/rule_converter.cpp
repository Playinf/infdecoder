/* input_converter.cpp */
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

template <class T>
void num2str(std::string& str, T num)
{
    std::stringstream ss;

    ss << num;

    str = ss.str();
}

bool is_nontermial(std::string& str)
{
    if (str[0] == '#')
        return true;

    return false;
}

void string_split(const std::string& s, const std::string& sep,
    std::vector<std::string>& vec)
{
    std::string::size_type len = s.length();
    std::string::size_type sep_len = sep.length();
    std::string::size_type start = 0;
    std::string::size_type pos = s.find(sep);
    std::string::size_type npos = s.npos;

    while (pos != npos) {
        std::string::size_type end = pos - start;

        if (end)
            vec.push_back(s.substr(start, end));

        start = pos + sep_len;
        pos = s.find(sep, start);
    }

    if (len > start)
        vec.push_back(s.substr(start, len));
}

void convert_nontermial(std::string& input, std::string& output)
{
    if (input.length() == 3)
        output = input;
    else {
        std::string str1 = input.substr(0, 2);
        std::string str2 = input.substr(2, 2);
        output = str1 + "_" + str2;
    }
}

void convert_rule(std::string& input, std::string& output)
{
    std::vector<std::string> vec;
    string_split(input, " ", vec);
    output += "#S# -> ";

    for (unsigned int i = 0; i < vec.size(); i++) {
        std::string& str = vec[i];

        if (is_nontermial(str)) {
            std::string tmp;
            convert_nontermial(str, tmp);
            output += tmp + " ";
        } else
            output += str + " ";
    }
}

void convert_prob(std::string& input, std::string& output)
{
    std::vector<std::string> vec;
    string_split(input, " ", vec);
    double alpha = 0.4342944819032518276511289189166;

    for (unsigned int i = 0; i < vec.size(); i++) {
        std::string& str = vec[i];
        std::string tmp;
        double prob = std::atof(str.c_str());
        prob *= alpha;
        num2str(tmp, prob);
        output += tmp + " ";
    }
}

void convert(std::ifstream& input_file, std::ofstream& output_file)
{
    std::string line;
    unsigned int id = 0;
    std::vector<std::string> vec;
    unsigned int count = 0;

    while (std::getline(input_file, line)) {
        std::string rule;
        std::string tmp;
        vec.clear();
        string_split(line, " ||| ", vec);
        std::string& src = vec[0];
        std::string& tgt = vec[1];
        std::string& prob = vec[2];
        convert_rule(src, tmp);
        rule += tmp + " ||| ";
        tmp.clear();
        convert_rule(tgt, tmp);
        rule += tmp + " ||| ";
        rule += "0-0 ||| ";
        tmp.clear();
        convert_prob(prob, tmp);
        rule += tmp +  " ||| 0 ";

        output_file << rule << std::endl;

        if (count && count % 100000 == 0)
            std::cout << count << std::endl;
        count++;
        line.clear();
    }
}

int main(int argc, char** argv)
{
    char* input_name;
    char* output_name;
    std::ifstream input;
    std::ofstream output;

    if (argc != 3) {
        std::cout << "invalid parameters" << std::endl;
        std::cout << "Usage: rconv input-name output-name" << std::endl;
        std::exit(-1);
    }

    input_name = argv[1];
    output_name = argv[2];

    input.open(input_name);
    output.open(output_name);

    if (input.fail()) {
        std::cout << "cannot find file `" << input_name << "'\n";
        std::exit(-1);
    } else if (output.fail()) {
        std::cout << "cannot find file `" << output_name << "'\n";
        std::exit(-1);
    }

    convert(input, output);

    input.close();
    output.close();

    return 0;
}
