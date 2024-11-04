#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include "card.hpp"

class Player
{
public:
    Player(std::string seed = "", std::string name = "");
    std::string getName() const;
    const Card *getCard() const;
    void setCard(const Card *card);
    std::string getId() const;
    virtual ~Player();

    bool operator==(const Player &p) const
    {
        return id == p.id;
    }

    bool operator==(const std::string &id) const
    {
        return this->id == id;
    }

    const std::string toJson(const bool &show_card = true) const;

private:
    std::string id;
    std::string name;
    const Card *card;
};

#endif