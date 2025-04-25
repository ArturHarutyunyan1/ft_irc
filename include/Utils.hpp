#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>

// long long stringToInt(std::string &str);
std::string welcomeMessage();
void setColor(int code);
void resetColor();
std::string helpMessage();
std::string passCommand(std::string msg, std::string password);
int string_to_number(std::string input);
std::string intToStr(int num);
std::string escapeJson(std::string input);

#endif