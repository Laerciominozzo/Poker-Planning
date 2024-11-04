#ifndef TABLE_STATE_HPP
#define TABLE_STATE_HPP
#include <string>
#include "table.hpp"
#include "task.hpp"
#include "player.hpp"
#include "err.hpp"

#define TASK_NO_DEFINED_STATE "NO_DEFINED"
#define WAITING_VOTES_STATE "WAITING_VOTES"
#define TASK_VOTED_STATE "TASK_VOTED"
#define TASK_FINISHED_STATE "TASK_FINISHED"

class Table;
class TableState
{
public:
    virtual ~TableState();
    virtual TableState *addTask(Table *table, Err *err, Task *task) = 0;
    virtual TableState *voteTask(Table *table, Err *err, Player &player, const Card &card) = 0;
    virtual TableState *revealTask(Table *table, Err *err) = 0;
    virtual TableState *cancelTask(Table *table, Err *err) = 0;
    virtual TableState *finishTable(Table *table, Err *err, std::string *report_ods) = 0;
    virtual TableState *removePlayer(Table *table, Err *err, const std::string &player_id) = 0;
    virtual TableState *addPlayer(Table *table, Err *err) = 0;
    virtual std::string toJson(const Table *table, const Player &player) const = 0;
};

class TableState_NoDefinedTask : public TableState
{
public:
    TableState_NoDefinedTask();
    virtual TableState *addTask(Table *table, Err *err, Task *task) override;
    virtual TableState *voteTask(Table *table, Err *err, Player &player, const Card &card) override;
    virtual TableState *revealTask(Table *table, Err *err) override;
    virtual TableState *cancelTask(Table *table, Err *err) override;
    virtual TableState *finishTable(Table *table, Err *err, std::string *report_ods) override;
    virtual TableState *removePlayer(Table *table, Err *err, const std::string &player_id) override;
    virtual TableState *addPlayer(Table *table, Err *err) override;
    virtual std::string toJson(const Table *table, const Player &player) const override;
    virtual ~TableState_NoDefinedTask();
};

class TableState_WaitingVotes : public TableState
{
public:
    TableState_WaitingVotes();
    virtual TableState *addTask(Table *table, Err *err, Task *task) override;
    virtual TableState *voteTask(Table *table, Err *err, Player &player, const Card &card) override;
    virtual TableState *revealTask(Table *table, Err *err) override;
    virtual TableState *cancelTask(Table *table, Err *err) override;
    virtual TableState *finishTable(Table *table, Err *err, std::string *report_ods) override;
    virtual TableState *removePlayer(Table *table, Err *err, const std::string &player_id) override;
    virtual TableState *addPlayer(Table *table, Err *err) override;
    virtual std::string toJson(const Table *table, const Player &player) const override;
    virtual ~TableState_WaitingVotes();
};

class TableState_TaskVoted : public TableState
{
public:
    TableState_TaskVoted();
    virtual TableState *addTask(Table *table, Err *err, Task *task) override;
    virtual TableState *voteTask(Table *table, Err *err, Player &player, const Card &card) override;
    virtual TableState *revealTask(Table *table, Err *err) override;
    virtual TableState *cancelTask(Table *table, Err *err) override;
    virtual TableState *finishTable(Table *table, Err *err, std::string *report_ods) override;
    virtual TableState *removePlayer(Table *table, Err *err, const std::string &player_id) override;
    virtual TableState *addPlayer(Table *table, Err *err) override;
    virtual std::string toJson(const Table *table, const Player &player) const override;
    virtual ~TableState_TaskVoted();
};

class TableState_TaskFinished : public TableState
{
public:
    TableState_TaskFinished();
    virtual TableState *addTask(Table *table, Err *err, Task *task) override;
    virtual TableState *voteTask(Table *table, Err *err, Player &player, const Card &card) override;
    virtual TableState *revealTask(Table *table, Err *err) override;
    virtual TableState *cancelTask(Table *table, Err *err) override;
    virtual TableState *finishTable(Table *table, Err *err, std::string *report_ods) override;
    virtual TableState *removePlayer(Table *table, Err *err, const std::string &player_id) override;
    virtual TableState *addPlayer(Table *table, Err *err) override;
    virtual std::string toJson(const Table *table, const Player &player) const override;
    virtual ~TableState_TaskFinished();
};

#endif