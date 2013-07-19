/* language_model.cpp */
#include <mutex>
#include <language_model.h>

language_model::language_model()
{
    id = 0;
    order = 0;
    feature_id = 0;
    type = SRI;
    model = nullptr;
}

language_model::language_model(const char* filename)
{
    load(filename);
}

language_model::~language_model()
{
    unload();
}

unsigned int language_model::get_id() const
{
    return id;
}

unsigned int language_model::get_order() const
{
    return order;
}

unsigned int language_model::get_feature_id() const
{
    return feature_id;
}

language_model_type language_model::get_type() const
{
    return type;
}

void language_model::load(const char* filename)
{
    model = sriLoadLM(filename, 1, order, 1, 0);
}

void language_model::unload()
{
    sriUnloadLM(model);
}

void language_model::set_id(unsigned int id)
{
    this->id = id;
}

void language_model::set_order(unsigned int n)
{
    order = n;
}

void language_model::set_feature(unsigned int id)
{
    feature_id = id;
}

void language_model::set_type(language_model_type type)
{
    this->type = type;
}

void language_model::ngram_probability(const input_type& str, float& full,
    float& ngram)
{
    std::unique_lock<std::mutex> lock { mutual_exclusion };
    std::string context;
    unsigned int offset = 0;
    unsigned int n = str.size();
    unsigned int context_len = 0;
    /* ln(10) */
    const float factor = 2.302585093;

    for (unsigned int i = 0; i < n; i++) {
        const std::string& word = *str[i];
        float prob;

        if (i == 0 && word == "<s>") {
            context += "<s> ";
            context_len += 1;
            continue;
        }

        const char* context_ptr = context.c_str() + offset;
        prob = sriWordProb(model, word.c_str(), context_ptr);

        if (context_len >= order - 1) {
            ngram += prob;
            offset += str[i - order + 1]->length() + 1;
        }
        full += prob;
        context += word;
        context += " ";
        context_len += 1;
    }

    full *= factor;
    ngram *= factor;
}

float language_model::word_probability(const std::string& word)
{
    /* ln(10) */
    std::unique_lock<std::mutex> lock { mutual_exclusion };
    const float factor = 2.302585093;

    if (word == "<s>")
        return 0.0;

    return factor * sriWordProb(model, word.c_str(), "");
}
