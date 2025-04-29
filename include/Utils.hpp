#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

// long long stringToInt(std::string &str);
std::string welcomeMessage();
void setColor(int code);
void resetColor();
std::string helpMessage();
std::string passCommand(std::string msg, std::string password);
int string_to_number(std::string input);
std::string intToStr(int num);
std::string escapeJson(std::string input);
int stringToInt(const std::string &string);
std::pair<std::string, std::string> getRequestBody(const std::string &msg);

#endif