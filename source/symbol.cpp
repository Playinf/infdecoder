#include <symbol.h>
#include <functional>

std::unordered_set<symbol, symbol_hash> symbol_table::symbol_set;
std::unordered_set<std::string> symbol_table::string_set;

symbol::symbol()
{
    name = nullptr;
    type = static_cast<symbol_type>(0);
}

symbol::symbol(const std::string& name, bool terminal)
{
    this->name = &name;

    if (terminal)
        this->type = TERMINAL;
    else
        this->type = NONTERMINAL;
}

symbol::symbol(const std::string& name, symbol_type type)
{
    this->name = &name;
    this->type = type;
}

symbol::~symbol()
{
    // do nothing
}

const std::string* symbol::get_name() const
{
    return name;
}

symbol_type symbol::get_type() const
{
    return type;
}

bool symbol::is_terminal() const
{
    return type == TERMINAL;
}

bool symbol::is_nonterminal() const
{
    return type != TERMINAL;
}

bool symbol::operator==(const symbol& sym) const
{
    if (type == sym.type) {
        return name == sym.name;
    }

    return false;
}

size_t symbol_hash::operator()(const symbol& sym) const
{
    unsigned long long address = (unsigned long long) sym.get_name();

    return std::hash<unsigned long long>()(address);
}

size_t symbol_hash::operator()(const symbol* sym) const
{
    if (sym == nullptr)
        return 0;

    return operator()(*sym);
}

const symbol* symbol_table::search_symbol(std::string& s)
{
    auto string_iterator = string_set.insert(s);
    const std::string& str = *string_iterator.first;

    symbol sym(str);

    auto result = symbol_set.insert(sym);

    return &(*(result.first));
}

const symbol* symbol_table::search_symbol(std::string& s, bool terminal)
{
    auto string_iterator = string_set.insert(s);
    const std::string& str = *string_iterator.first;
    symbol sym(str, terminal);

    auto result = symbol_set.insert(sym);

    return &(*(result.first));
}
