#ifndef SERVER_HPP
# define SERVER_HPP

#include "check.hpp"
#include <string>
#include <iostream>

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

        void test();
};

#endif
