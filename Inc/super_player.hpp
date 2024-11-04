#ifndef SUPER_PLAYER_HPP
#define SUPER_PLAYER_HPP
#include <string>
#include <vector>
#include "player.hpp"
#include "table.hpp"
#include "task.hpp"

class Table;
class SuperPlayer : public Player
{
public:
    SuperPlayer(std::string seed, std::string name);
    virtual ~SuperPlayer();

    void addTask(Table *table, Task *task);
    void revealTask(Table *table);
    void finishTable(Table *table, std::string *report_ods);
    void cancelTask(Table *table);
    void removePlayer(Table *table, const std::string player_id);
};
#endif