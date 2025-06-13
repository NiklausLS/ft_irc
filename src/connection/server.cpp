#include "../includes/check.hpp"
#include "../includes/server.hpp"

//TO DO LIST
//1 - lancer le serveur (en cour)
//2 - sockets
//3 - connexions
// ????

Server::Server() : _port(0), _password(""), _socket(-1)
{
    std::cout << YELLOW << "SERVER CONSTRUCTOR" << RESET << std::endl;
}

Server::Server(int port, const std::string& password) : _port(port), _password(password), _socket(-1)
{
    std::cout << BLUE << "---- port = " << _port << RESET << std::endl;
}

Server::Server(const Server &copy)
{
    _port = copy._port;
    _password = copy._password;
    _socket = -1;
}

Server& Server::operator=(const Server &copy)
{
    if (this != &copy)
    {
        _port = copy._port;
        _password = copy._password;
        _socket = -1;
    }
    return *this;
}

// Destructeur
Server::~Server()
{
    std::cout << YELLOW << "SERVER DESTRUCTOR" << RESET << std::endl;
}

void Server::test()
{
    std::cout << GREEN << "--- SERVER STARTING ---" << RESET << std::endl;
    std::cout << "Port: " << _port << std::endl;
    std::cout << "Password: " << _password << std::endl;

    std::cout << GREEN << "--- SERVER TEST END ---" << RESET << std::endl;
}