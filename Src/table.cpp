#include "table.hpp"
#include <stdio.h>
#include <algorithm>
#include <syslog.h>
#include "util.hpp"

Table::Table(std::string seed, std::string name, const Deck *deck, std::string super_player_name)
{
    this->id = generateId(seed, name);
    this->seed = seed;
    this->name = name;
    TableState_NoDefinedTask *state = new TableState_NoDefinedTask();
    this->state = dynamic_cast<TableState *>(state);
    this->current_task = NULL;
    this->super_player = new SuperPlayer(seed, super_player_name);
    this->players.push_back(super_player);
    this->deck = deck;
}

Table::~Table()
{
    for (auto player : players)
    {
        delete player;
    }
    players.clear();
    delete state;
    // delete current_task;
    for (auto task : voted_tasks)
    {
        delete task;
    }
    voted_tasks.clear();
    state = NULL;
}

const std::string Table::getId() const
{
    return id;
}

const std::string Table::getSuperPlayerId() const
{
    return super_player->getId();
}

const std::string Table::getName() const { return this->name; }

/**
 * Verifica se todos os jogadores  votaram em uma tarafa.
 *
 * @return true se todos os jogadores votaram, false caso contr rio.
 */
bool Table::isAllVotesReceived()
{
    return std::all_of(players.begin(), players.end(), [](Player *player)
                       { return player->getCard() != NULL; });
}

/**
 * Adiciona um jogador  a mesa.
 *
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 * @param player_name Nome do jogador que ser  adicionado.
 * @param new_player_id Ser  preenchido com o identificador do jogador recem adicionado.
 */
void Table::addPlayer(Err *err, const std::string &player_name, std::string &new_player_id)
{

    syslog(LOG_INFO, "Adicionando jogador: %s", player_name.c_str());

    Player *player = new Player(this->seed, player_name);
    auto player_exists = std::find_if(players.begin(), players.end(), [player](Player *p)
                                      { return *p == *player; });

    if (player_exists != players.end())
    {
        err->code = ERR_CODE_BAD_REQUEST;
        err->msg = "Jogador ja existe na mesa";
        syslog(LOG_ERR, "Jogador %s ja existe na mesa.", player_name.c_str());
        return;
    }

    players.push_back(player);
    TableState *new_state = this->state->addPlayer(this, err);
    if (new_state != this->state)
    {
        delete this->state;
        this->state = new_state;
    }
    new_player_id = player->getId();

    if (!err)
    {
        syslog(LOG_INFO, "Jogador adicionado: %s", player_name.c_str());
    }
    else
    {
        syslog(LOG_ERR, "Erro ao adicionar jogador: %s", player_name.c_str());
    }
}

/**
 * Adiciona uma tarafa  a mesa.
 *
 * @param player_id Identificador do jogador que adicionou a tarafa.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem  de erro.
 * @param task     Tarafa que ser  adicionada.
 */
void Table::addTask(const std::string &player_id, Err *err, Task *task)
{

    syslog(LOG_INFO, "Adicionando tarafa: %s", task->label.c_str());
    if (this->super_player->getId() != player_id)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Operação não permitida";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }

    TableState *state = this->state->addTask(this, err, task);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Tarafa adicionada: %s", task->label.c_str());
    }
    else
    {
        syslog(LOG_ERR, "Erro ao adicionar tarafa: %s", err->msg.c_str());
    }
}

/**
 * Registra um voto em uma tarafa.
 *
 * @param player_id Identificador do jogador que realizou o voto.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 * @param card_label Label da carta que ser  usada para o voto.
 */
void Table::voteTask(const std::string &player_id, Err *err, const std::string &card_label)
{
    syslog(LOG_INFO, "Votando na tarafa: %s", card_label.c_str());
    const Card *card = deck->getCard(card_label);
    if (card == NULL)
    {
        err->code = ERR_CODE_NOT_FOUND;
        err->msg = "Carta não encontrada";
        return;
    }

    auto player = std::find_if(players.begin(), players.end(), [player_id](Player *p)
                               { return *p == player_id; });

    if (player == players.end())
    {
        err->code = ERR_CODE_BAD_REQUEST;
        err->msg = "Jogador não cadastrado nesta sala";
        syslog(LOG_ERR, "Jogador %s não existe na mesa.", player_id.c_str());
        return;
    }
    syslog(LOG_INFO, "Jogador %s", (*player)->getName().c_str());

    TableState *state = this->state->voteTask(this, err, **player, *card);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Voto registrado na tarafa: %s", card_label.c_str());
    }
    else
    {
        syslog(LOG_ERR, "Erro ao registrar voto na tarafa: %s", err->msg.c_str());
    }
}

/**
 * Revela a tarafa.
 *
 * @param player_id Identificador do jogador que realizou a operação.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 */
void Table::revealTask(const std::string &player_id, Err *err)
{

    syslog(LOG_INFO, "Revelando tarafa");
    if (this->super_player->getId() != player_id)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Operação não permitida";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }

    TableState *state = this->state->revealTask(this, err);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Tarafa revelada");
    }
    else
    {
        syslog(LOG_ERR, "Erro ao revelar tarafa: %s", err->msg.c_str());
    }
}

/**
 * Cancela a tarafa.
 *
 * @param player_id Identificador do jogador que realizou a operação.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 */
void Table::cancelTask(const std::string &player_id, Err *err)
{

    syslog(LOG_INFO, "Cancelando tarafa");
    if (this->super_player->getId() != player_id)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Operação não permitida";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }

    TableState *state = this->state->cancelTask(this, err);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Tarafa cancelada");
    }
    else
    {
        syslog(LOG_ERR, "Erro ao cancelar tarafa: %s", err->msg.c_str());
    }
}

/**
 * Finaliza a mesa, gerando um relatorio em formato ODS no buffer 'report_ods'.
 *
 * @param player_id Identificador do jogador que realizou a operação.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 * @param report_ods Ser  preenchido com o relatorio em formato ODS.
 */
void Table::finishTable(const std::string &player_id, Err *err, std::string *report_ods)
{

    syslog(LOG_INFO, "Finalizando mesa");
    if (this->super_player->getId() != player_id)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Operação não permitida";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }

    TableState *state = this->state->finishTable(this, err, report_ods);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Mesa finalizada");
    }
    else
    {
        syslog(LOG_ERR, "Erro ao finalizar mesa: %s", err->msg.c_str());
    }
}

/**
 * Remove um jogador da mesa.
 *
 * @param player_id Identificador do jogador que realizou a remo a operação.
 * @param err      Ponteiro para estrutura de erro. Caso ocorra um erro, a estrutura
 *                 de erro ser  preenchida com a mensagem e c digo de erro.
 * @param player_id_to_remove Identificador do jogador a ser removido.
 */
void Table::removePlayer(const std::string &player_id, Err *err, const std::string &player_id_to_remove)
{

    syslog(LOG_INFO, "Removendo jogador: %s", player_id_to_remove.c_str());
    if (this->super_player->getId() != player_id)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Operação não permitida";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }
    if (this->super_player->getId() == player_id_to_remove)
    {
        err->code = ERR_CODE_NOT_ALLOWED;
        err->msg = "Não pode remover o super-usuário";
        syslog(LOG_ERR, "Operação não permitida pelo jogador: %s", player_id.c_str());
        return;
    }

    TableState *state = this->state->removePlayer(this, err, player_id_to_remove);
    if (state != this->state)
    {
        delete this->state;
        this->state = state;
    }

    if (!err)
    {
        syslog(LOG_INFO, "Jogador removido");
    }
    else
    {
        syslog(LOG_ERR, "Erro ao remover jogador: %s", err->msg.c_str());
    }
}

const std::string Table::toJson(Err *err, const std::string &player_id) const
{
    syslog(LOG_INFO, "Gerando JSON do estado da mesa");
    auto player = std::find_if(players.begin(), players.end(), [player_id](Player *p)
                               { return *p == player_id; });

    if (player == players.end())
    {
        err->code = ERR_CODE_BAD_REQUEST;
        err->msg = "Jogador não cadastrado nesta sala";
        syslog(LOG_ERR, "Jogador %s não existe na mesa.", player_id.c_str());
        return "";
    }

    return this->state->toJson(this, **player);
}