#include "views.hpp"
#include "main.hpp"
#include "table.hpp"
#include "util.hpp"
#include <stdio.h>
#include <map>
#include "err.h"
#include <stdio.h>

void initUrls(crow::SimpleApp &app)
{

  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)([](const crow::request &req)
                                                      {
                                                        auto page = crow::mustache::load_text("initialPage.html");
                                                        return crow::response(ERR_CODE_OK, page); });

  // Start room POST page
  CROW_ROUTE(app, "/")
      .methods(crow::HTTPMethod::POST)(
          [](const crow::request &req)
          {
            std::map<std::string, std::string> form_data;
            decodeBody(req.body, form_data);
            std::string room_label = form_data["room_label"];
            std::string user_label = form_data["user_label"];

            decodeBody(req.body, form_data);
            char seed[RANDOM_SEED_LENGTH];
            getrandom(seed, sizeof(seed));
            std::string seed_str(seed, sizeof(seed));

            std::string room_id, user_id;
            Table *table = new Table(seed_str, room_label, &deck, user_label);
            room_id = table->getId();
            user_id = table->getSuperPlayerId();
            Table *existent_table = getTable(room_id);
            if (existent_table != NULL)
            {
              return crow::response(ERR_CODE_BAD_REQUEST, std::string("Table already exists"));
            }
            all_tables.push_back(table);

            // Redirect to Room page GET
            auto rsp = crow::response(302, "Found");
            rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
            return rsp;
          });

  // Room page GET
  CROW_ROUTE(app, "/room/<string>/<string>")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req, std::string room_id, std::string user_id)
                                      {
    Table *table = getTable(room_id);

    if (table == NULL)
    {
      return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
    }

    std::string url = req.get_header_value("Host");
    std::string http("http://");
    url = http + url;
    crow::mustache::context context = { {"table_label", table->getName()},
                                        {"isAdmin", table->getSuperPlayerId() == user_id ? true : false},
                                        {"status_link", url + "/state/" + room_id + "/" + user_id},
                                        {"table_link", url + "/room/" + room_id},
                                        {"new_task_link", url + "/task/" + room_id + "/" + user_id},
                                        {"vote_link", url + "/vote/" + room_id + "/" + user_id},
                                        {"reveal_link", url + "/reveal/" + room_id + "/" + user_id},
                                        {"remove_link", url + "/remove/" + room_id + "/" + user_id},
                                        {"cancel_link", url + "/cancel/" + room_id + "/" + user_id} };

        auto page = crow::mustache::load("roomPage.html");
        // Render the Mustache template with the context
 
        std::string rendered_template = page.render_string( context);

        // Return the rendered template as an HTML response
        return crow::response(ERR_CODE_OK, rendered_template); });

  // Create User page GET
  CROW_ROUTE(app, "/room/<string>")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req, const std::string room_id)
                                      {
    Table *table = getTable(room_id);
    if (table == NULL) {
      return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
    }

    crow::mustache::context context = {{"table_label",table->getName()},};
    auto page = crow::mustache::load("userPage.html");
    // Render the Mustache template with the context
    std::string rendered_template = page.render_string(context);
    // Return the rendered template as an HTML response
    return crow::response(ERR_CODE_OK, rendered_template); });

  // Create User page POST
  CROW_ROUTE(app, "/room/<string>")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req, const std::string room_id)
                                       {
    Table *table = getTable(room_id);
    if (table == NULL) {
      return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
    }
    std::map<std::string, std::string> form_data;
    decodeBody(req.body, form_data);
    std::string user_label = form_data["user_label"];
    std::string new_palyer_id;
    Err err;
    table->addPlayer(&err, user_label, new_palyer_id);
    if (err) {
      return crow::response(err.code, err.msg);
    }
    // Redirect to Room page GET
    auto rsp = crow::response(302, "Found");
    rsp.add_header("Location", "/room/" + room_id + "/" + new_palyer_id);
    return rsp; });

  // Get Table State Json
  CROW_ROUTE(app, "/state/<string>/<string>")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req, std::string room_id, std::string user_id)
                                      {
    Table *table = getTable(room_id);
    if (table == NULL)
    {
      return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
    }

    Err err;
    std::string json = table->toJson(&err, user_id);

    if (err)
    {
      return crow::response(err.code, "text/plain", err.msg);
    }
    return crow::response(ERR_CODE_OK, json); });

  // Create a task
  CROW_ROUTE(app, "/task/<string>/<string>")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req, std::string room_id, std::string user_id)
                                       {
    Table *table = getTable(room_id);
    if (table == NULL)
    {
      return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
    }
    std::map<std::string, std::string> form_data;
    decodeBody(req.body, form_data);
    std::string task_name = form_data["task_name"];
    Task *task = new Task(task_name);
    Err err;
    table->addTask(user_id, &err, task);
    if (err)
    {
      return crow::response(err.code, "text/plain", err.msg);
    }
    // Redirect to Room page GET
    auto rsp = crow::response(302, "Found");
    rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
    return rsp; });

  // Vote task
  CROW_ROUTE(app, "/vote/<string>/<string>")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req, std::string room_id, std::string user_id)
                                       {
        Table *table = getTable(room_id);
        if (table == NULL)
        {
            return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
        }

        // Parse JSON string
        crow::json::rvalue json = crow::json::load(req.body);

        // Get a JSON element
        std::string vote = json["vote"].s();
        Err err;
        table->voteTask(user_id, &err, vote);
        if (err)
        {
            return crow::response(err.code, "text/plain", err.msg);
        }
        // Redirect to Room page GET
        auto rsp = crow::response(302, "Found");
        rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
        return rsp; });

  // remove User
  CROW_ROUTE(app, "/remove/<string>/<string>")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req, std::string room_id, std::string user_id)
                                       {
        Table *table = getTable(room_id);
        if (table == NULL)
        {
            return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
        }
          // Parse JSON string
        crow::json::rvalue json = crow::json::load(req.body);

        // Get a JSON element
        std::string player_id = json["id"].s();
        Err err;
        table->removePlayer(user_id, &err, player_id);
        if (err)
        {
            return crow::response(err.code, "text/plain", err.msg);
        }
        // Redirect to Room page GET
        auto rsp = crow::response(302, "Found");
        rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
        return rsp; });

  // Cancel task
  CROW_ROUTE(app, "/cancel/<string>/<string>")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req, std::string room_id, std::string user_id)
                                      {
        Table *table = getTable(room_id);
        if (table == NULL)
        {
            return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
        }
        Err err;
        table->cancelTask(user_id, &err);
        if (err)
        {
            return crow::response(err.code, "text/plain", err.msg);
        }
        // Redirect to Room page GET
        auto rsp = crow::response(302, "Found");
        rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
        return rsp; });

  // Reveal task
  CROW_ROUTE(app, "/reveal/<string>/<string>")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req, std::string room_id, std::string user_id)
                                      {
        Table *table = getTable(room_id);
        if (table == NULL)
        {
            return crow::response(ERR_CODE_NOT_FOUND, "Table not found");
        }
        Err err;
        table->revealTask(user_id, &err);
        if (err)
        {
            return crow::response(err.code, "text/plain", err.msg);
        }
        // Redirect to Room page GET
        auto rsp = crow::response(302, "Found");
        rsp.add_header("Location", "/room/" + room_id + "/" + user_id);
        return rsp; });
}
