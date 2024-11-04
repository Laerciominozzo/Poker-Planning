#include "card.hpp"

Card::Card(std::string label, float value) : label(label), value(value) {}

const std::string Card::toJson() const
{
    return "{\"label\":\"" + label + "\"}";
}