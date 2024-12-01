#ifndef MAIN_HPP
#define MAIN_HPP

#include "card.hpp"
#include "deck.hpp"
#include "err.hpp"
#include "table.hpp"
#include <string>
#include <vector>

const std::vector<Card> cards = {Card("1", 1), Card("2", 2), Card("3", 3),
                                 Card("5", 5), Card("8", 8), Card("13", 13),
                                 Card("20", 20), Card("40", 40), Card("80", 80),
                                 Card("?", -1)};
extern std::vector<Table *> all_tables;
const Deck deck = Deck(cards);

Table *getTable(const std::string &table_id);
std::string joinTable(const std::string &table_id,
                      const std::string &player_name, Err *err);

#endif