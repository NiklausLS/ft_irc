#include "check.hpp"
#include "server.hpp"
#include "message.hpp"  

//TO DO LIST
//1 - lancer le serveur (OK)
//2 - sockets (OK ?)
//3 - connexions (en cours)
// ????

Server::Server() : _port(0), _password(""), _socket(-1)
{
    std::cout << YELLOW << "SERVER CONSTRUCTOR" << RESET << std::endl;
}

Server::~Server()
{
    std::cout << YELLOW << "SERVER DESTRUCTOR" << RESET << std::endl;
    if (_socket != -1)
    {
        close(_socket);
        _socket = -1;
    }
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

void Server::exec()
{
    std::cout << GREEN << "--- SERVER STARTING ---" << RESET << std::endl;

    makeSocket();
    getClient();

    std::cout << "Password = " << _password << std::endl;
    std::cout << GREEN << "--- SERVER OK ---" << RESET << std::endl;
}

void Server::makeSocket()
{
    std::cout << GREEN << "--- SERVER STARTING ---" << RESET << std::endl;
    std::cout << BLUE << "--- TRY MAKING SOCKET" << RESET << std::endl;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        std::cerr << RED << "ERROR: socket creation failed" << RESET << std::endl;
        return ;
    }
    std::cout << GREEN << "Socket OK fd = " << _socket << RESET << std::endl;
    std::cout << "Port: " << _port << std::endl;
    std::cout << "Password: " << _password << std::endl;
    std::cout << "------------\n" << std::endl;

    //pour bind au port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    std::cout << BLUE << "--- TRY MAKING BIND" << RESET << std::endl;
    if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << RED << "ERROR: bind failed port = " << _port << RESET << std::endl;
        close(_socket);
        _socket = -1;
        return;
    }
    std::cout << GREEN << "Bind OK port = " << _port << RESET << std::endl;
    std::cout << "------------\n" << std::endl;
    
    //listen pour les connexions
    std::cout << BLUE << "--- TRY MAKING LISTEN" << RESET << std::endl;
    if (listen(_socket, 5) == -1)
    {
        std::cerr << RED << "ERROR: listen failed" << RESET << std::endl;
        return;
    }
    std::cout << GREEN << "Listen OK port = " << _port << RESET << std::endl;
    std::cout << "------------\n" << std::endl;

    std::cout << GREEN << "--- SERVER TEST END ---" << RESET << std::endl;
}

void Server::getClient()
{
    std::cout << BLUE << "--- WAITING FOR CLIENT ---" << RESET << std::endl;
    
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    int clientSocket = accept(_socket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket == -1)
    {
        std::cerr << RED << "ERROR: accept failed" << RESET << std::endl;
        return;
    }
    
    std::cout << GREEN << "client fd = " << clientSocket << RESET << std::endl;
    
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        std::cout << PINK << "Received = " << buffer << RESET << std::endl;
        
        //PAS PARFAITE
        parseMessage(std::string(buffer));

        std::string response = "Echo = ";
        response += buffer;
        response += "\r\n";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    else
        std::cout << YELLOW << "Client sent no data" << RESET << std::endl;
    close(clientSocket);
    std::cout << YELLOW << "Client disconnected" << RESET << std::endl;
}

void Server::parseMessage(const std::string& input)
{
    std::cout << YELLOW << "--- PARSING MESSAGE ---" << RESET << std::endl;
    
    Message ircMessage;
    if (ircMessage.parseSimple(input))
    {
        std::string command = ircMessage.getCommand();
        std::cout << GREEN << "Parsed command = " << command << RESET << std::endl;
    }
    else
        std::cout << RED << "ERROR: parsing message" << input << RESET << std::endl;
}