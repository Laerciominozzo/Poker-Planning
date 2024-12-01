#include "util.hpp"
#include <openssl/sha.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <curl/curl.h>

/**
 * Generates a SHA256 hash of the given seed and identifier,
 * and returns a string containing the first ID_LENGTH bytes
 * of the hash.
 *
 * @param seed       A string to be used as the seed for the hash.
 * @param identifier The identifier to be hashed.
 *
 * @return A string of length ID_LENGTH that uniquely identifies
 *         the given identifier.
 */
std::string generateId(std::string seed, std::string identifier)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, seed.c_str(), seed.length());
    SHA256_Update(&sha256, identifier.c_str(), identifier.length());
    SHA256_Final(hash, &sha256);

    // convert to hexa string
    char hexa[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&hexa[i * 2], "%02X", hash[i]);
    return std::string((char *)hexa, ID_LENGTH);
}

int getrandom(char *buffer, int length)
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    ssize_t len = read(fd, buffer, length);
    if (len != length)
    {
        perror("read");
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

void replaceAll(std::string &str, const std::string &oldChar, const std::string &newChar)
{
    size_t pos = str.find(oldChar);
    while (pos != std::string::npos)
    {
        str.replace(pos, oldChar.length(), newChar);
        pos = str.find(oldChar, pos + 1);
    }
}
std::string decodeURL(std::string &str)
{
    std::string ret;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        int decodelen;
        char *decoded = curl_easy_unescape(curl, str.c_str(), str.length(), &decodelen);
        if (decoded)
        {
            ret = std::string(decoded, decodelen);
            curl_free(decoded);
        }
        curl_easy_cleanup(curl);
    }
    return ret;
}
int decodeBody(std::string body, std::map<std::string, std::string> &form_data)
{

    // Replace + to space
    replaceAll(body, "+", " ");

    size_t pos = 0;
    while ((pos = body.find("&")) != std::string::npos)
    {
        auto pair = body.substr(0, pos);
        size_t equal_pos = pair.find("=");
        if (equal_pos != std::string::npos)
        {
            auto key = pair.substr(0, equal_pos);
            auto value = pair.substr(equal_pos + 1);
            form_data[key] = decodeURL(value);
        }
        body.erase(0, pos + 1);
    }

    // Handle the last key-value pair
    size_t equal_pos = body.find("=");
    if (equal_pos != std::string::npos)
    {
        auto key = body.substr(0, equal_pos);
        auto value = body.substr(equal_pos + 1);

        form_data[key] = decodeURL(value);
    }
    return 0;
}