#include "../include/Utils.hpp"

void setColor(int code)
{
    std::cout << "\033[" << code << "m";
}

void resetColor()
{
    std::cout << "\033[0m";
}

std::string welcomeMessage()
{
    std::string msg = "\033[31m// ···············································································\\\n"
              "// :____    __    ____  _______  __        ______   ______   .___  ___.  _______ :\\\n"
              "// :\\   \\  /  \\  /   / |   ____||  |      /      | /  __  \\  |   \\/   | |   ____|:\\\n"
              "// : \\   \\/    \\/   /  |  |__   |  |     |  ,----'|  |  |  | |  \\  /  | |  |__   :\\\n"
              "// :  \\            /   |   __|  |  |     |  |     |  |  |  | |  |\\/|  | |   __|  :\\\n"
              "// :   \\    /\\    /    |  |____ |  `----.|  `----.|  `--'  | |  |  |  | |  |____ :\\\n"
              "// :    \\__/  \\__/     |_______||_______| \\______| \\______/  |__|  |__| |_______|:\\\n"
              "// ···············································································\n"
              "\n                         Use HELP command if you need any help                          \033[0m\n";
    return (msg);
}

std::string helpMessage()
{
    std::string msg = "\033[32m\n"
                      "STEP 1: SET PASSWORD\n"
                      "\033[38;5;214m        Use PASS command to set a password. e.g: PASS [SERVER_PASSWORD]\n"
                      "\033[32mSTEP 2: NICK\n"
                      "\033[38;5;214m         Use NICK command to set a nickname. e.g: NICK yourNickname\n"
                      "\033[32mSTEP 3: USER\n"
                      "\033[38;5;214m         Use USER command to register your username and full name. e.g: USER name * * :EXAMPLE\033[0m\n";
    return (msg);
}

