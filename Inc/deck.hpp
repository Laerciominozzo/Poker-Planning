#ifndef DECK_HPP
#define DECK_HPP
#include "card.hpp"
#include <string>
#include <vector>

class Deck
{

private:
  const std::vector<Card> cards;

public:
  Deck(const std::vector<Card> &cards);
  const Card *getCard(const std::string &card) const;
  const std::string toJson() const;
};

#endif