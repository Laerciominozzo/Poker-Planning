#ifndef ERR_HPP
#define ERR_HPP
#include <string>

#define ERR_CODE_OK 200
#define ERR_CODE_NOT_ALLOWED 405
#define ERR_CODE_NOT_FOUND 404
#define ERR_CODE_INTERNAL_SERVER_ERROR 500
#define ERR_CODE_BAD_REQUEST 400
#define ERR_CODE_FORBIDDEN 403
class Err
{
public:
    std::string msg = "";
    uint32_t code = ERR_CODE_OK;

    operator bool() const
    {
        return code != ERR_CODE_OK;
    }
};
#endif