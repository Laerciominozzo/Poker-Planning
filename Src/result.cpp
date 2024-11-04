#include "result.hpp"

void Result::addVote(const Card &card)
{
    votes[card]++;
}

int Result::getMedian() const
{
    float median = 0;
    int votes = 0;

    if (this->votes.size() == 0)
    {
        return 0;
    }

    for (auto &v : this->votes)
    {
        median += v.first.value * v.second;
        votes += v.second;
    }

    median /= votes;
    return median;
}

const std::string Result::toJson() const
{
    std::string json = "{\"votes\":[";
    bool first = true;
    const std::pair<const Card, int> *more_voted = &(*votes.begin());
    for (auto &v : votes)
    {
        if (v.second == 0)
        {
            continue;
        }

        if (v.second > more_voted->second)
        {
            more_voted = &v;
        }

        if (!first)
        {
            json += ",";
        }
        json += std::string("{\"card\":\"") + v.first.label + "\",\"votes\":" + std::to_string(v.second) + "}";
        first = false;
    }
    json += "]";
    // calculate median
    json += ",\"median\":" + std::to_string(getMedian());

    json += "}";
    return json;
}

const std::string Result::toODS() const
{
    std::string ods = "";
    for (auto &v : votes)
    {
        ods += v.first.label + " " + std::to_string(v.second) + "\n";
    }
    return ods;
}