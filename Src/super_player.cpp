#include "super_player.hpp"

SuperPlayer::SuperPlayer(std::string seed, std::string name) : Player(seed, name) {}

SuperPlayer::~SuperPlayer() {}

void SuperPlayer::addTask(Table *table, Task *task)
{

    table->current_task = task;

    // clear the player's card
    for (auto it = table->players.begin(); it != table->players.end(); it++)
    {
        (*it)->setCard(NULL);
    }
}

void SuperPlayer::revealTask(Table *table)
{
    // store all votes in the task result
    for (Player *p : table->players)
    {
        table->current_task->result.addVote(*p->getCard());
    }

    if (table->current_task != nullptr)
    {
        table->voted_tasks.insert(table->voted_tasks.begin(), table->current_task);
    }
}

void SuperPlayer::finishTable(Table *table, std::string *report_ods)
{

    report_ods->clear();
    *report_ods += "Name\tValue\n";
    for (auto task : table->voted_tasks)
    {
        *report_ods += task->label + "\t";
        *report_ods += task->result.toODS() + "\n";
    }

    table->current_task = nullptr;
    table->voted_tasks.clear();
}

void SuperPlayer::cancelTask(Table *table)
{
    Task *current_task = table->current_task;

    if (current_task != nullptr)
    {
        table->current_task->canceled = true;
        table->voted_tasks.insert(table->voted_tasks.begin(), current_task);
    }

    table->current_task = nullptr;
}

void SuperPlayer::removePlayer(Table *table, std::string player_id)
{
    for (auto it = table->players.begin(); it != table->players.end(); it++)
    {
        if (**it == player_id)
        {
            delete *it;
            table->players.erase(it);

            break;
        }
    }
}
