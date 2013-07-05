#include <vector>
#include <iostream>
#include <rule_tree.h>

void moses_format_loader(void* args);

int main()
{
    const char* rule_table = "rules.txt";
    const char* glue_grammar = "glue-grammar";
    void** arg_arr = new void*[2];
    rule_tree table;

    arg_arr[0] = (void*) rule_table;
    arg_arr[1] = (void*) &table;
    moses_format_loader((void*) arg_arr);

    std::cout << "rule number: " << table.get_rule_number() << std::endl;
    std::cout << "node number: " << table.get_node_number() << std::endl;

    delete[] arg_arr;
}
