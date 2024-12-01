#ifndef UTIL_HPP
#define UTIL_HPP
#include <string>
#include <map>

#define ID_LENGTH 8
#define RANDOM_SEED_LENGTH 32

std::string generateId(std::string seed, std::string identifier);

int getrandom(char *buffer, int length);

int decodeBody(std::string body, std::map<std::string, std::string> &form_data);

#endif