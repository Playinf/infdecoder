/* utility.cpp */
#include <sstream>
#include <utility.h>

std::size_t sring_hash(const char* str)
{
    const char *name = str;
    unsigned int h = 0;
    unsigned int g;

    while (*name) {
        h = (h << 4) + *name++;
        if ((g = (h & 0xf0000000)))
            h ^= g >> 24;
        h &= ~g;
    }

    return h;
}

std::size_t data_hash(const char* data, size_t len)
{
    std::size_t res = 0;
    const char* p = data;

    while (len--)
        res = (res << 1) ^ *p++;

    return res;
}

std::size_t hash_combine(std::size_t v1, std::size_t v2)
{
    std::size_t val = 0;

    val ^= v2 + 0x9e3779b9 + (v1 << 6) + (v1 >> 2);

    return val;
}

void string_join(std::vector<std::string>& vec, const std::string& sep,
    std::string& out)
{
    unsigned int size = vec.size();

    if (size == 0)
        return;

    std::stringstream sstream;

    sstream << vec[0];

    for (unsigned int i = 1; i < size; i++) {
        sstream << sep << vec[i];
    }

    out = sstream.str();
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

int string_vector_compare(const std::vector<const std::string*>* s1,
    const std::vector<const std::string*>* s2)
{
    unsigned int size1 = s1->size();
    unsigned int size2 = s2->size();
    unsigned int size = size1 > size2 ? size2 : size1;

    for (unsigned int i = 0; i < size; i++) {
        const std::string* str1 = s1->at(i);
        const std::string* str2 = s2->at(i);
        int result;

        result = str1->compare(*str2);

        if (result != 0)
            return result;
    }

    if (size1 > size2)
        return 1;
    else if (size1 < size2)
        return -1;

    return 0;
}

void string_trim(const std::string& str, std::string& ret)
{
    std::string::size_type pos1 = str.find_first_not_of(' ');
    std::string::size_type pos2 = str.find_last_not_of(' ');
    std::string::size_type npos = std::string::npos;

    if (pos1 == npos || pos2 == npos)
        ret = str;
    else
        ret = str.substr(pos1, pos2 - pos1 + 1);
}
