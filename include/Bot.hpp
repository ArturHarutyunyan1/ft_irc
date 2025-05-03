#ifndef BOT_HPP
# define BOT_HPP

#include <netinet/in.h>
#include <string>
#include "openssl/ssl.h"
#include "openssl/err.h"

enum State
{
    DEFAULT,
    INIT,
    HANDSHAKE,
    SENDING,
    RECEIVING,
    COMPLETE
};

class Bot
{
    private:
        State _state;
        SSL_CTX* _sslCtx;
        SSL* _ssl;
        std::string _request;
        std::string _response;
        int _clientFd;

    public:
        Bot();
        ~Bot();

        void initSocket();
        void cleanup();

        State getState();
        void setState(State state);
        
        SSL_CTX* getSslCtx();
        void setSslCtx(SSL_CTX* ctx);
        
        SSL* getSsl();
        void setSsl(SSL* ssl);
        
        std::string& getRequest();
        void setRequest(std::string req);

        std::string& getResponse();
        void setResponse(std::string resp);

        void setClientFd(int fd);
        int getClientFd();
};

#endif