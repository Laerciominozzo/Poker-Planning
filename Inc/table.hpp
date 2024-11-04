#ifndef TABLE_HPP
#define TABLE_HPP
#include "crow.h"
#include "deck.hpp"
#include "err.hpp"
#include "player.hpp"
#include "super_player.hpp"
#include "table_state.hpp"
#include "task.hpp"
#include <string>
#include <vector>

class SuperPlayer;
class TableState;
class Table
{
protected:
  std::string id;
  std::string seed;
  std::string name;
  std::vector<Player *> players;
  std::vector<Task *> voted_tasks;
  TableState *state;
  Task *current_task;
  SuperPlayer *super_player;
  const Deck *deck;

  bool isAllVotesReceived();

public:
  friend class SuperPlayer;
  friend class TableState_NoDefinedTask;
  friend class TableState_TaskVoted;
  friend class TableState_TaskFinished;
  friend class TableState_WaitingVotes;

  const std::string toJson(Err *err, const std::string &player_id) const;

  Table(std::string seed, std::string name, const Deck *deck,
        std::string super_player_name);

  const std::string getId() const;
  const std::string getSuperPlayerId() const;
  const std::string getName() const;

  // Functions called externally
  void addPlayer(Err *err, const std::string &player_name,
                 std::string &new_player_id);
  void addTask(const std::string &player_id, Err *err, Task *task);
  void voteTask(const std::string &player_id, Err *err,
                const std::string &card_label);
  void revealTask(const std::string &player_id, Err *err);
  void cancelTask(const std::string &player_id, Err *err);
  void finishTable(const std::string &player_id, Err *err,
                   std::string *report_ods);
  void removePlayer(const std::string &player_id, Err *err,
                    const std::string &player_id_to_remove);
  ~Table();
};

#endif