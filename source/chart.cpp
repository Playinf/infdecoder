/* chart.cpp */
#include <chart.h>

chart::chart(size_type len)
{
    size = len;
    table = new chart_cell*[size];

    for (size_type i = 0; i < size; i++)
        table[i] = new chart_cell[size - i];
}

chart::~chart()
{
    for (size_type i = 0; i < size; i++)
        delete[] table[i];
    delete table;
}

chart_cell* chart::get_cell(size_type start_pos, size_type end_pos) const
{
    return &table[start_pos][end_pos - start_pos];
}
