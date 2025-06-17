#ifndef SERVER_HPP
# define SERVER_HPP

#include "check.hpp"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 

class Server
{
    private:
        int _port;
        std::string _password;
        int _socket;

    public:
        Server();
        Server(int port, const std::string &password);
        ~Server();
        Server(const Server &copy);
        Server& operator=(const Server &copy);

        void makeSocket();
        void exec();
        void getClient();

        void parseMessage(const std::string &input);
};

#endif
