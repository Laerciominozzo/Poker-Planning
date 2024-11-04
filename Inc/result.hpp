#ifndef RESULT_HPP
#define RESULT_HPP
#include <map>
#include <string>
#include "card.hpp"

class Result
{
    std::map<Card, int> votes;
    int getMedian() const;

public:
    void addVote(const Card &card);
    const std::string toJson() const;
    const std::string toODS() const;
};

#endif