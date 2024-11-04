#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include "result.hpp"

class Task
{

public:
    const std::string label;
    Result result;
    bool canceled = false;
    Task(std::string label);
    Task();
    const std::string toJson(const bool &show_result) const;
    ~Task();
};

#endif