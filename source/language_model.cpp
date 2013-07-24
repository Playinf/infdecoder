/* language_model.cpp */
#include <mutex>
#include <srilm.h>
#include <language_model.h>

language_model::language_model()
{
    id = 0;
    order = 0;
    feature_id = 0;
    type = SRI;
    model = new srilm();
}

language_model::language_model(const char* filename)
{
    id = 0;
    order = 0;
    feature_id = 0;
    type = SRI;
    model = new srilm();
    load(filename);
}

language_model::~language_model()
{
    delete model;
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
    model->load(filename);
}

void language_model::set_id(unsigned int id)
{
    this->id = id;
}

void language_model::set_order(unsigned int n)
{
    order = n;
    model->set_order(order);
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
    std::vector<std::string*> context;
    unsigned int offset = 0;
    unsigned int n = str.size();
    unsigned int context_len = 0;
    /* ln(10) */
    const float factor = 2.302585093;

    for (unsigned int i = 0; i < n; i++) {
        std::string* word = const_cast<std::string*>(str[i]);
        float prob;

        if (i == 0 && *word == "<s>") {
            context.push_back(word);
            context_len += 1;
            continue;
        }

        std::string** context_ptr = context.data() + offset;

        prob = model->word_probability(word, context_ptr, context_len);

        if (context_len >= order - 1) {
            ngram += prob;
            offset += 1;
            context_len -= 1;
        }
        full += prob;
        context.push_back(word);
        context_len += 1;
    }

    full *= factor;
    ngram *= factor;
}

float language_model::word_probability(const std::string& word)
{
    /* ln(10) */
    const float factor = 2.302585093;
    std::string* w = const_cast<std::string*>(&word);

    if (word == "<s>")
        return 0.0;

    return factor * model->word_probability(w, nullptr, 0);
}
