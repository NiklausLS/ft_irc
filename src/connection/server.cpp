#include "check.hpp"
#include "server.hpp"
#include "message.hpp"  

//TO DO LIST
//1 - lancer le serveur (OK)
//2 - sockets (OK ?)
//3 - connexions (en cours)
// LISTEN BIND SELECT
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
    makeServer();
    //makeSocket();

    /*while (true)
    {
        getClient();
    }*/

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

    //BIND AU PORT
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
    std::cout << GREEN << "BIND OK port = " << _port << RESET << std::endl;
    std::cout << "------------\n" << std::endl;
    
    //LISTEN LES CO
    std::cout << BLUE << "--- TRY MAKING LISTEN" << RESET << std::endl;
    if (listen(_socket, 5) == -1)
    {
        std::cerr << RED << "ERROR: listen failed" << RESET << std::endl;
        return;
    }
    std::cout << GREEN << "LISTEN OK port = " << _port << RESET << std::endl;
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
    
    char buffer[70000];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        std::cout << PINK << "received = " << buffer << RESET << std::endl;
        
        //PAS PARFAITE
        parseMessage(std::string(buffer));

        std::string response = "Echo = ";
        response += buffer;
        response += "\r\n";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    else
        std::cout << YELLOW << "client sent no data" << RESET << std::endl;
    close(clientSocket);
    std::cout << YELLOW << "client disconnected" << RESET << std::endl;
}

void Server::parseMessage(const std::string& input)
{
    std::cout << YELLOW << "--- PARSING MESSAGE ---" << RESET << std::endl;
    
    Message ircMessage;
    if (ircMessage.parseSimple(input))
    {
        std::string command = ircMessage.getCommand();
        std::cout << GREEN << "parsed command = " << command << RESET << std::endl;
    }
    else
        std::cout << RED << "ERROR: parsing message" << input << RESET << std::endl;
}

void Server::makeServer()
{
   std::cout << GREEN << "--- MAKING SERVER ---" << RESET << std::endl;
   
   makeSocket();
   
   std::cout << BLUE << "--- STARTING SERVER LOOP ---" << RESET << std::endl;
   
   while (true)
   {
       std::cout << "------------" << std::endl;
       std::cout << YELLOW << "--- PREPARING SELECT ---" << RESET << std::endl;
       
       fd_set readfds;
       FD_ZERO(&readfds);
       std::cout << "FD_ZERO done" << std::endl;
       
       FD_SET(_socket, &readfds);
       std::cout << "server socket " << _socket << " added to readfds" << std::endl;
       
       int maxfd = _socket;
       std::cout << "initial maxfd = " << maxfd << std::endl;
       
       std::cout << "clients count = " << _clients.size() << std::endl;
       for (size_t i = 0; i < _clients.size(); ++i)
       {
           std::cout << "adding client " << i << " fd = " << _clients[i] << " to readfds" << std::endl;
           FD_SET(_clients[i], &readfds);
           if (_clients[i] > maxfd)
           {
               maxfd = _clients[i];
               std::cout << "new maxfd = " << maxfd << std::endl;
           }
       }
       
       std::cout << YELLOW << "--- SELECT with maxfd = " << maxfd << RESET << std::endl;
       
       int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
       std::cout << GREEN << "select = " << activity << RESET << std::endl;
       
       if (activity > 0)
       {
           std::cout << BLUE << "--- CHECKING SERVER SOCKET ---" << RESET << std::endl;
           if (FD_ISSET(_socket, &readfds))
           {
               std::cout << "new connection" << std::endl;
               int newClient = accept(_socket, NULL, NULL);
               if (newClient != -1)
               {
                   _clients.push_back(newClient);
                   std::cout << GREEN << "new client and fd = " << newClient << RESET << std::endl;
                   std::cout << "nbr of clients = " << _clients.size() << std::endl;
               }
               else
                   std::cout << RED << "accept failed" << RESET << std::endl;
           }
           else
               std::cout << "no new connection" << std::endl;
           
           std::cout << BLUE << "--- START CHECKING CLIENT MESSAGES ---" << RESET << std::endl;
           for (size_t i = 0; i < _clients.size(); ++i)
           {
               std::cout << "checking client " << i << " fd = " << _clients[i] << std::endl;
               if (FD_ISSET(_clients[i], &readfds))
               {
                   std::cout << PINK << "-- client " << _clients[i] << " has data" << RESET << std::endl;
                   char buffer[512];
                   int bytesReceived = recv(_clients[i], buffer, sizeof(buffer) - 1, 0);
                   std::cout << "recv res = " << bytesReceived << " bytes" << std::endl;
                   
                   if (bytesReceived > 0)
                   {
                       buffer[bytesReceived] = '\0';
                       std::cout << PINK << "- message from client " << _clients[i] << " = " << buffer << RESET << std::endl;
                       
                       parseMessage(std::string(buffer));
                       
                       std::string response = "TEST CMD = ";
                       response += buffer;
                       response += "\r\n";
                       
                       std::cout << "--- START SEND : response to client " << _clients[i] << std::endl;
                       int sentBytes = send(_clients[i], response.c_str(), response.length(), 0);
                       std::cout << GREEN << "-- SEND res = " << sentBytes << " to client " << _clients[i] << RESET << std::endl;
                   }
                   else if (bytesReceived == 0)
                   {
                       std::cout << YELLOW << "- client " << _clients[i] << " disconnection OK" << RESET << std::endl;
                       close(_clients[i]);
                       std::cout << "closed client fd " << _clients[i] << "OK" << std::endl;
                       
                       _clients.erase(_clients.begin() + i);
                       std::cout << "* ERASE from clients list. Remaining = " << _clients.size() << std::endl;
                       break;
                   }
                   else
                   {
                       std::cout << RED << "recv error for client " << _clients[i] << RESET << std::endl;
                       close(_clients[i]);
                       _clients.erase(_clients.begin() + i);
                       std::cout << "ERROR: erase client now. Remaining = " << _clients.size() << std::endl;
                       break;
                   }
               }
               else
                   std::cout << "* client " << _clients[i] << " has no data" << std::endl;
           }
           std::cout << "--- END CHECK CLIENTS --- " << std::endl;
        }
        else
           std::cout << RED << "SELECT KO : error ???" << RESET << std::endl;      
        std::cout << GREEN << "--- END OF LOOP ITERATION ---" << RESET << std::endl;
   }
}