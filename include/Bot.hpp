#ifndef BOT_HPP
# define BOT_HPP

#include <openssl/ssl.h>
#include <string>

enum State
{
    IDLE,
    WRITING,
    READING
};

class Bot
{
    private:
        int _socketFd;
        State _state;
    public:
        Bot();
        ~Bot();

        void initSocket();
        void cleanup();

        int getSocketFd();
        State   getState();
};

#endif