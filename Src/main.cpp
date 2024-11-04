#include "crow.h"
#include "views.hpp"
// #include "crow_all.h"
#include "main.hpp"

std::vector<Table *> all_tables;

Table *getTable(const std::string &table_id)
{
  for (auto it = all_tables.begin(); it != all_tables.end(); it++)
  {
    if ((*it)->getId() == table_id)
    {
      return (*it);
    }
  }
  return NULL;
}

std::string joinTable(const std::string &table_id,
                      const std::string &player_name, Err *err)
{
  Table *table = getTable(table_id);
  if (table == NULL)
  {
    err->code = ERR_CODE_NOT_FOUND;
    err->msg = "Sala não encontrada";
    return "";
  }
  std::string player_id;
  table->addPlayer(err, player_name, player_id);
  return player_id;
}

int main()
{
  crow::SimpleApp app; // define your crow application

  // define your routes
  initUrls(app);

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  int amountOfTables = all_tables.size();
  for (int i = 0; i < amountOfTables; i++)
  {
    delete all_tables[i];
  }
  all_tables.clear();

  return 0;
}