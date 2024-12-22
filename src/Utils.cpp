#include "../include/Utils.hpp"

int stringToLong(const std::string &str)
{
    std::stringstream ss(str);
    int result;

    ss >> result;
    if (ss.fail() || !ss.eof() || result > std::numeric_limits<int>::max())
    {
        throw std::exception();
        return (0);
    }
    return (result);
}