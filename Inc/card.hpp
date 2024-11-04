#ifndef CARD_HPP
#define CARD_HPP
#include <string>
class Card
{
public:
    const std::string label;
    const float value;
    Card(std::string label, float value);
    const std::string toJson() const;

    inline bool operator<(const Card &other) const
    {
        if (value == other.value)
        {
            return label < other.label;
        }
        return value < other.value;
    }
};

#endif