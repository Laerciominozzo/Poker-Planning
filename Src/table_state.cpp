#include "table_state.hpp"
#include <syslog.h>

TableState::~TableState() {}
//============================================================================
// TableState_NoDefinedTask
//============================================================================

TableState_NoDefinedTask::TableState_NoDefinedTask()
{
    syslog(LOG_INFO, "Aguardando nova tarafa");
}

TableState_NoDefinedTask::~TableState_NoDefinedTask() {}

TableState *TableState_NoDefinedTask::addTask(Table *table, Err *err, Task *task)
{
    table->super_player->addTask(table, task);
    return new TableState_WaitingVotes();
}

TableState *TableState_NoDefinedTask::voteTask(Table *table, Err *err, Player &player, const Card &card)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "Não há tarafa para votar agora";
    return this;
}
TableState *TableState_NoDefinedTask::revealTask(Table *table, Err *err)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "Não há tarafa para revelar agora";
    return this;
}
TableState *TableState_NoDefinedTask::cancelTask(Table *table, Err *err)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "Não há tarafa para cancelar agora";
    return this;
}
TableState *TableState_NoDefinedTask::finishTable(Table *table, Err *err, std::string *report_ods)
{
    table->super_player->finishTable(table, report_ods);
    return this;
}

TableState *TableState_NoDefinedTask::removePlayer(Table *table, Err *err, const std::string &player_id)
{
    table->super_player->removePlayer(table, player_id);
    return this;
}

TableState *TableState_NoDefinedTask::addPlayer(Table *table, Err *err)
{
    return this;
}

std::string TableState_NoDefinedTask::toJson(const Table *table, const Player &player) const
{
    std::string json = "{\"players\":[";
    for (auto player : table->players)
    {
        json += player->toJson(false);
        json += ",";
    }
    if (table->players.size() > 0)
        json.pop_back();
    json += "]";

    // Show current state
    json += ",\"state\":\"" TASK_NO_DEFINED_STATE "\"";

    // show deck
    json += ",\"deck\":" + table->deck->toJson();

    // Show voted tasks
    json += ",\"tasks\":[";

    for (auto task : table->voted_tasks)
    {
        json += task->toJson(true);
        json += ",";
    }
    if (table->voted_tasks.size() > 0)
        json.pop_back();
    json += "]";

    // show  player card
    if (player.getCard() != nullptr)
    {
        json += ",\"card\":" + player.getCard()->toJson();
    }

    json += "}";

    return json;
}

//============================================================================
// TableState_WaitingVotes
//============================================================================

TableState_WaitingVotes::TableState_WaitingVotes()
{
    syslog(LOG_INFO, "Aguarando votos");
}
TableState_WaitingVotes::~TableState_WaitingVotes() {}

TableState *TableState_WaitingVotes::addTask(Table *table, Err *err, Task *task)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "É preciso cancelar a tarafa antes de adicionar outra";
    return this;
}
TableState *TableState_WaitingVotes::voteTask(Table *table, Err *err, Player &player, const Card &card)
{
    player.setCard(&card);

    if (table->isAllVotesReceived())
    {
        return new TableState_TaskVoted();
    }
    else
    {
        return this;
    }
}
TableState *TableState_WaitingVotes::revealTask(Table *table, Err *err)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "Todos os jogadores precisam votar antes de revelar";
    return this;
}
TableState *TableState_WaitingVotes::cancelTask(Table *table, Err *err)
{
    table->super_player->cancelTask(table);
    return new TableState_NoDefinedTask();
}
TableState *TableState_WaitingVotes::finishTable(Table *table, Err *err, std::string *report_ods)
{
    table->super_player->finishTable(table, report_ods);
    return new TableState_NoDefinedTask();
}

TableState *TableState_WaitingVotes::removePlayer(Table *table, Err *err, const std::string &player_id)
{
    table->super_player->removePlayer(table, player_id);

    if (table->isAllVotesReceived())
    {
        return new TableState_TaskVoted();
    }

    return this;
}

TableState *TableState_WaitingVotes::addPlayer(Table *table, Err *err)
{

    if (table->isAllVotesReceived())
    {
        return new TableState_TaskVoted();
    }

    return this;
}

std::string TableState_WaitingVotes::toJson(const Table *table, const Player &player) const
{
    std::string json = "{\"players\":[";
    for (auto player : table->players)
    {
        json += player->toJson(false);
        json += ",";
    }
    if (table->players.size() > 0)
        json.pop_back();
    json += "]";

    // Show current state
    json += ",\"state\":\"" WAITING_VOTES_STATE "\"";

    // show deck
    json += ",\"deck\":" + table->deck->toJson();

    // Show current task
    json += ",\"task\":" + table->current_task->toJson(false);

    // Show voted tasks
    json += ",\"tasks\":[";

    for (auto task : table->voted_tasks)
    {
        json += task->toJson(true);
        json += ",";
    }
    if (table->voted_tasks.size() > 0)
        json.pop_back();
    json += "]";

    // show  player card
    if (player.getCard() != nullptr)
    {
        json += ",\"card\":" + player.getCard()->toJson();
    }

    json += "}";

    return json;
}

//============================================================================
// TableState_TaskVoted
//============================================================================

TableState_TaskVoted::TableState_TaskVoted()
{
    syslog(LOG_INFO, "Votação concluída");
}
TableState_TaskVoted::~TableState_TaskVoted() {}

TableState *TableState_TaskVoted::addTask(Table *table, Err *err, Task *task)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "Revele ou cancele a tarafa antes de adicionar outra";
    return this;
}
TableState *TableState_TaskVoted::voteTask(Table *table, Err *err, Player &player, const Card &card)
{
    player.setCard(&card);

    bool all_voted = std::all_of(table->players.begin(), table->players.end(), [](Player *p)
                                 { return p->getCard() != NULL; });

    if (all_voted)
    {
        return this;
    }
    else
    {
        return new TableState_WaitingVotes();
    }
}
TableState *TableState_TaskVoted::revealTask(Table *table, Err *err)
{
    table->super_player->revealTask(table);
    return new TableState_TaskFinished();
}
TableState *TableState_TaskVoted::cancelTask(Table *table, Err *err)
{
    table->super_player->cancelTask(table);
    return new TableState_NoDefinedTask();
}
TableState *TableState_TaskVoted::finishTable(Table *table, Err *err, std::string *report_ods)
{
    table->super_player->finishTable(table, report_ods);
    return new TableState_NoDefinedTask();
}

TableState *TableState_TaskVoted::removePlayer(Table *table, Err *err, const std::string &player_id)
{
    table->super_player->removePlayer(table, player_id);

    if (table->isAllVotesReceived())
    {
        return this;
    }
    return new TableState_WaitingVotes();
}

TableState *TableState_TaskVoted::addPlayer(Table *table, Err *err)
{

    if (table->isAllVotesReceived())
    {
        return this;
    }

    return new TableState_WaitingVotes();
}

std::string TableState_TaskVoted::toJson(const Table *table, const Player &player) const
{
    std::string json = "{\"players\":[";
    for (auto player : table->players)
    {
        json += player->toJson(false);
        json += ",";
    }
    if (table->players.size() > 0)
        json.pop_back();
    json += "]";

    // Show current state
    json += ",\"state\":\"" TASK_VOTED_STATE "\"";

    // show deck
    json += ",\"deck\":" + table->deck->toJson();

    // Show current task
    json += ",\"task\":" + table->current_task->toJson(false);

    // Show voted tasks
    json += ",\"tasks\":[";

    for (auto task : table->voted_tasks)
    {
        json += task->toJson(true);
        json += ",";
    }
    if (table->voted_tasks.size() > 0)
        json.pop_back();
    json += "]";

    // show  player card
    if (player.getCard() != nullptr)
    {
        json += ",\"card\":" + player.getCard()->toJson();
    }

    json += "}";

    return json;
}

//============================================================================
// TableState_TaskFinished
//============================================================================

TableState_TaskFinished::TableState_TaskFinished()
{
    syslog(LOG_INFO, "Tarefa concluída");
}
TableState_TaskFinished::~TableState_TaskFinished() {}

TableState *TableState_TaskFinished::addTask(Table *table, Err *err, Task *task)
{
    table->super_player->addTask(table, task);
    return new TableState_WaitingVotes();
}
TableState *TableState_TaskFinished::voteTask(Table *table, Err *err, Player &player, const Card &card)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "A tarefa não pode ser mais receber votos";
    return this;
}
TableState *TableState_TaskFinished::revealTask(Table *table, Err *err)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "A tarefa não pode ser mais revelada";
    return this;
}
TableState *TableState_TaskFinished::cancelTask(Table *table, Err *err)
{
    err->code = ERR_CODE_NOT_ALLOWED;
    err->msg = "A tarefa já foi votada e não pode ser mais cancelada";
    return this;
}
TableState *TableState_TaskFinished::finishTable(Table *table, Err *err, std::string *report_ods)
{
    table->super_player->finishTable(table, report_ods);
    return new TableState_NoDefinedTask();
}

TableState *TableState_TaskFinished::removePlayer(Table *table, Err *err, const std::string &player_id)
{
    table->super_player->removePlayer(table, player_id);
    return this;
}

TableState *TableState_TaskFinished::addPlayer(Table *table, Err *err)
{
    return this;
}

std::string TableState_TaskFinished::toJson(const Table *table, const Player &player) const
{
    std::string json = "{\"players\":[";
    for (auto player : table->players)
    {
        json += player->toJson(true);
        json += ",";
    }
    if (table->players.size() > 0)
        json.pop_back();
    json += "]";

    // Show current state
    json += ",\"state\":\"" TASK_FINISHED_STATE "\"";

    // show deck
    json += ",\"deck\":" + table->deck->toJson();

    // Show current task
    json += ",\"task\":" + table->current_task->toJson(true);

    // Show voted tasks
    json += ",\"tasks\":[";

    for (auto task : table->voted_tasks)
    {
        json += task->toJson(true);
        json += ",";
    }
    if (table->voted_tasks.size() > 0)
        json.pop_back();
    json += "]";

    // show  player card
    if (player.getCard() != nullptr)
    {
        json += ",\"card\":" + player.getCard()->toJson();
    }

    json += "}";

    return json;
}