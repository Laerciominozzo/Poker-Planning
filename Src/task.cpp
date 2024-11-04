#include "task.hpp"

Task::Task(std::string label) : label(label) {}

const std::string Task::toJson(const bool &show_result) const
{
    if (show_result)
    {
        return "{\"label\":\"" + label + "\",\"result\":" + result.toJson() + "}";
    }
    else
    {
        return "{\"label\":\"" + label + "\"}";
    }
}
Task::~Task() {}