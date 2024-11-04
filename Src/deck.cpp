#include "deck.hpp"
#include "card.hpp"

Deck::Deck(const std::vector<Card> &cards) : cards(cards) {}

const Card *Deck::getCard(const std::string &card) const
{
  for (auto &c : cards)
  {
    if (c.label == card)
    {
      return &c;
    }
  }
  return nullptr;
}

const std::string Deck::toJson() const
{
  std::string json = "[";
  for (auto &c : cards)
  {
    json += c.toJson();
    json += ",";
  }
  if (cards.size() > 0)
    json.pop_back();
  json += "]";
  return json;
}