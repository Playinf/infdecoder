/* test_symbol_table.cpp */
#include <ctime>
#include <thread>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <symbol.h>
using namespace std;

unsigned int num = 26;

void test_thread(const vector<string>& vec)
{
    int count = 0;
    int pos;
    int num = vec.size();
    symbol_table* table = symbol_table::get_instance();
    srand(time(NULL));

    while (count < 10000) {
        pos = rand() % num;
        string sym(vec[pos]);
        const symbol* s = table->search_symbol(sym);
        //cout << *s->get_name() << endl;
        count++;
    }
}

int main()
{
    vector<string> vec;

    for (int i = 0; i < 100; i++) {
        int val = rand();
        vec.push_back(to_string(val));
    }

    symbol_table* table = symbol_table::get_instance();

    thread t1 { test_thread, vec };
    thread t2 { test_thread, vec };
    thread t3 { test_thread, vec };
    thread t4 { test_thread, vec };

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << std::endl;
    std::cout << table->size() << std::endl;
}
