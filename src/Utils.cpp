#include "../include/Utils.hpp"
#include <sstream>
#include <iostream>

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

std::string escapeJson(std::string input)
{
    std::string result;

    for (size_t i = 0; i < input.length(); i++)
    {
        char c = input[i];
        if (c == '\"')
            result += "\\\"";
        else if (c == '\\')
            result += "\\\\";
        else if (c == '\n')
            result += "\\n";
        else
            result += c;
    }
    return result;
}

std::string intToStr(int num)
{
    std::ostringstream os;

    os << num;

    return os.str();
}

int stringToInt(const std::string &string)
{
    std::stringstream ss(string);

	int res;
	ss >> res;
	return (res);
}

std::pair<std::string, std::string> getRequestBody(const std::string &msg)
{
    std::string host = "api.groq.com";
    std::string apiKey = "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";
    std::string endpoint = "/openai/v1/chat/completions";
    std::string contentType = "Content-Type: application/json";
    std::string bearer = "Authorization: Bearer " + apiKey;
    std::string connection = "Connection: close";

    std::string escapedMsg = escapeJson(msg);
    std::string body =
        "{\n"
        "\"model\": \"llama-3.3-70b-versatile\",\n"
        "\"messages\": [{\"role\": \"user\", \"content\": \"" + escapedMsg + "\"}]\n"
        "}";

    std::string message =
        "POST " + endpoint + " HTTP/1.1\n"
        "Host: " + host + "\n"
        + contentType + "\n"
        + bearer + "\n"
        + connection + "\n"
        "Content-Length: " + intToStr(body.length()) + "\n"
        + "\n"
        + body;

    std::pair<std::string, std::string> temp;
    
    temp.first = message;
    temp.second = host;

    return temp;
}