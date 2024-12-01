#include "player.hpp"
#include "card.hpp"
#include "util.hpp"

Player::Player(std::string seed, std::string name)
{
    this->name = name;
    this->id = generateId(seed, name);
    this->card = NULL;
}

void Player::setCard(const Card *card)
{
    if (card == this->card)
    {
        this->card = NULL;
        return;
    }
    this->card = card;
}

const Card *Player::getCard() const
{
    return card;
}

std::string Player::getName() const
{
    return name;
}

std::string Player::getId() const
{
    return id;
}

const std::string Player::toJson(const bool &show_card) const
{
    if (card == NULL)
    {
        return "{\"id\":\"" + id + "\",\"name\":\"" + name + "\",\"card\":\"-\"}";
    }

    if (!show_card)
    {
        return "{\"id\":\"" + id + "\",\"name\":\"" + name + "\",\"card\":\"#\"}";
    }

    return "{\"id\":\"" + id + "\",\"name\":\"" + name + "\",\"card\":\"" + this->card->label + "\"}";
}
Player::~Player() {}