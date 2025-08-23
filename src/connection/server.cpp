#include "check.hpp"
#include "server.hpp"
#include "message.hpp"  
#include "client.hpp"

//TO DO LIST
//1 - lancer le serveur (OK)
//2 - sockets (OK ?)
//3 - connexions (en cours)
// LISTEN BIND SELECT
// ????

Server::Server() : _port(0), _password(""), _socket(-1)
{
    //std::cout << YELLOW << "SERVER CONSTRUCTOR" << RESET << std::endl;
}

Server::~Server()
{
    //std::cout << YELLOW << "SERVER DESTRUCTOR" << RESET << std::endl;
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
    return (*this);
}

void Server::exec()
{
    //std::cout << GREEN << "--- SERVER STARTING ---" << RESET << std::endl;
    makeServer();

    //std::cout << "Password = " << _password << std::endl;
    std::cout << GREEN << "--- SERVER OK ---" << RESET << std::endl;
}

void Server::makeSocket()
{
    std::cout << GREEN << "- SOCKET CREATION START" << RESET << std::endl;
    //std::cout << BLUE << "-- TRY MAKING SOCKET" << RESET << std::endl;
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        std::cerr << RED << "--- ERROR: socket creation failed" << RESET << std::endl;
        return ;
    }
    std::cout << GREEN << "--- Socket OK fd = " << _socket << RESET << std::endl;

    //BIND SECTION
    //std::cout << BLUE << "- BIND START" << RESET << std::endl;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    //std::cout << BLUE << "-- TRY MAKING BIND" << RESET << std::endl;
    if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << RED << "--- ERROR: bind failed port = " << _port << RESET << std::endl;
        close(_socket);
        _socket = -1;
        return ;
    }
    //std::cout << GREEN << "--- BIND OK port = " << _port << RESET << std::endl;
    
    //LISTEN SECTION
    //std::cout << BLUE << "- LISTEN START" << RESET << std::endl;
    //std::cout << BLUE << "-- TRY MAKING LISTEN" << RESET << std::endl;
    if (listen(_socket, 5) == -1)
    {
        std::cerr << RED << "--- ERROR: listen failed" << RESET << std::endl;
        return ;
    }
    //std::cout << GREEN << "--- LISTEN OK port = " << _port << RESET << std::endl;
    //std::cout << GREEN << "---- SOCKET SETUP COMPLETE" << RESET << std::endl;
}

void Server::parseMessage(const std::string& input)
{
    //std::cout << YELLOW << "- PARSING START" << RESET << std::endl;
    
    Message ircMessage;
    if (ircMessage.parseSimple(input))
    {
        std::string command = ircMessage.getCommand();
        std::cout << GREEN << "-- parsed command = " << command << RESET << std::endl;
    }
    else
        std::cout << RED << "-- ERROR: parsing message" << input << RESET << std::endl;
}

/*void Server::makeServer()
{
   //std::cout << GREEN << "- SERVER INIT" << RESET << std::endl;
   
   makeSocket();
   
   //std::cout << BLUE << "-- STARTING SERVER LOOP" << RESET << std::endl;
   
   while (true)
   {
       // SELECT PREPARATION SECTION
       //std::cout << YELLOW << "- SELECT PREP START" << RESET << std::endl;
       
       fd_set readfds;
       FD_ZERO(&readfds);
       //std::cout << "-- FD_ZERO done" << std::endl;
       
       FD_SET(_socket, &readfds);
       //std::cout << "--- server socket " << _socket << " added to readfds" << std::endl;
       
       int maxfd = _socket;
       //std::cout << "---- initial maxfd = " << maxfd << std::endl;
       
       //std::cout << "- CLIENT FD ADDING" << std::endl;
       //std::cout << "-- clients count = " << _clients.size() << std::endl;
       for (size_t i = 0; i < _clients.size(); ++i)
       {
           //std::cout << "--- adding client " << i << " fd = " << _clients[i]->getFd() << " to readfds" << std::endl;
           FD_SET(_clients[i]->getFd(), &readfds);
           if (_clients[i]->getFd() > maxfd)
           {
               maxfd = _clients[i]->getFd();
               //std::cout << "---- new maxfd = " << maxfd << std::endl;
           }
       }
       
       // SELECT CALL SECTION
       //std::cout << YELLOW << "- SELECT CALL" << RESET << std::endl;
       //std::cout << "-- calling select with maxfd = " << maxfd << std::endl;
       
       int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
       //std::cout << GREEN << "--- select returned = " << activity << RESET << std::endl;
       
       if (activity > 0)
       {
           // NEW CONNECTION SECTION
           //std::cout << BLUE << "- NEW CONNECTION CHECK" << RESET << std::endl;
           if (FD_ISSET(_socket, &readfds))
           {
               //std::cout << "-- new connection detected" << std::endl;
               int newClient = accept(_socket, NULL, NULL);
               if (newClient != -1)
               {
                   Client* clientObj = new Client(newClient);
                   _clients.push_back(clientObj);
                   //std::cout << GREEN << "--- new client created fd = " << newClient << RESET << std::endl;
                   //std::cout << "---- total clients = " << _clients.size() << std::endl;
               }
               else
                   std::cout << RED << "--- accept failed" << RESET << std::endl;
           }
           else
               std::cout << "-- no new connection" << std::endl;
           
           // CLIENT MESSAGE SECTION
           //std::cout << BLUE << "- CLIENT MESSAGE CHECK" << RESET << std::endl;
           for (size_t i = 0; i < _clients.size(); ++i)
           {
               //std::cout << "-- checking client " << i << " fd = " << _clients[i]->getFd() << std::endl;
               if (FD_ISSET(_clients[i]->getFd(), &readfds))
               {
                   //std::cout << PINK << "--- client " << _clients[i]->getFd() << " has data" << RESET << std::endl;
                   
                   // RECV SECTION
                   //std::cout << "- RECV DATA" << std::endl;
                   char buffer[512];
                   int bytesReceived = recv(_clients[i]->getFd(), buffer, sizeof(buffer) - 1, 0);
                   //std::cout << "-- recv returned = " << bytesReceived << " bytes" << std::endl;
                   
                   if (bytesReceived > 0)
                   {
                       buffer[bytesReceived] = '\0';
                       std::cout << PINK << "--- message content = " << buffer << RESET << std::endl;
                       
                       // BUFFER SECTION
                       //std::cout << "- BUFFER PROCESSING" << std::endl;
                       _clients[i]->addData(std::string(buffer));
                       
                       // PARSING SECTION
                       parseMessage(std::string(buffer));
                       
                       // SEND SECTION
                       //std::cout << "- SEND RESPONSE" << std::endl;
                       std::string response = "SERVER ECHO = ";
                       response += buffer;
                       response += "\r\n";
                       
                       //std::cout << "-- sending to client " << _clients[i]->getFd() << std::endl;
                       int sentBytes = send(_clients[i]->getFd(), response.c_str(), response.length(), 0);
                       std::cout << GREEN << "--- sent " << sentBytes << " bytes" << RESET << std::endl;
                   }
                   else if (bytesReceived == 0)
                   {
                       // DISCONNECT SECTION
                       std::cout << YELLOW << "- CLIENT DISCONNECT" << RESET << std::endl;
                       std::cout << "-- client " << _clients[i]->getFd() << " disconnected cleanly" << std::endl;
                       close(_clients[i]->getFd());
                       delete (_clients[i]);
                       std::cout << "--- client object deleted" << std::endl;
                       
                       _clients.erase(_clients.begin() + i);
                       std::cout << "---- remaining clients = " << _clients.size() << std::endl;
                       break ;
                   }
                   else
                   {
                       // ERROR SECTION
                       std::cout << RED << "- CLIENT ERROR" << RESET << std::endl;
                       std::cout << "-- recv error for client " << _clients[i]->getFd() << std::endl;
                       close(_clients[i]->getFd());
                       delete (_clients[i]);
                       _clients.erase(_clients.begin() + i);
                       std::cout << "--- ERROR:client removed and remaining = " << _clients.size() << std::endl;
                       break ;
                   }
               }
               else
                   std::cout << "--- client " << _clients[i]->getFd() << " has no data" << std::endl;
           }
           //std::cout << "-- client check OK" << std::endl;
        }
        else
           std::cout << RED << "- SELECT ERROR" << RESET << std::endl;      
        
        std::cout << GREEN << "- LOOP ITERATION OK" << RESET << std::endl;
        std::cout << "********************" << std::endl;
   }
}*/

void Server::makeServer()
{
   //std::cout << GREEN << "- SERVER INIT" << RESET << std::endl;
   
   makeSocket();
   
   //std::cout << BLUE << "-- STARTING SERVER LOOP" << RESET << std::endl;
   
   while (true)
   {
       // SELECT PREPARATION SECTION
       //std::cout << YELLOW << "- SELECT PREP START" << RESET << std::endl;
       
       fd_set readfds;
       FD_ZERO(&readfds);
       //std::cout << "-- FD_ZERO done" << std::endl;
       
       FD_SET(_socket, &readfds);
       //std::cout << "--- server socket " << _socket << " added to readfds" << std::endl;
       
       int maxfd = _socket;
       //std::cout << "---- initial maxfd = " << maxfd << std::endl;
       
       //std::cout << "- CLIENT FD ADDING" << std::endl;
       //std::cout << "-- clients count = " << _clients.size() << std::endl;
       for (size_t i = 0; i < _clients.size(); ++i)
       {
           //std::cout << "--- adding client " << i << " fd = " << _clients[i]->getFd() << " to readfds" << std::endl;
           FD_SET(_clients[i]->getFd(), &readfds);
           if (_clients[i]->getFd() > maxfd)
           {
               maxfd = _clients[i]->getFd();
               //std::cout << "---- new maxfd = " << maxfd << std::endl;
           }
       }
       
       // SELECT CALL SECTION
       //std::cout << YELLOW << "- SELECT CALL" << RESET << std::endl;
       //std::cout << "-- calling select with maxfd = " << maxfd << std::endl;
       
       int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
       std::cout << GREEN << "--- select returned = " << activity << RESET << std::endl;
       
       if (activity > 0)
       {
           // NEW CONNECTION SECTION
           //std::cout << BLUE << "- NEW CONNECTION CHECK" << RESET << std::endl;
           if (FD_ISSET(_socket, &readfds))
           {
               //std::cout << "-- new connection detected" << std::endl;
               int newClient = accept(_socket, NULL, NULL);
               if (newClient != -1)
               {
                   Client* clientObj = new Client(newClient);
                   _clients.push_back(clientObj);
                   //std::cout << GREEN << "--- new client created fd = " << newClient << RESET << std::endl;
                   //std::cout << "---- total clients = " << _clients.size() << std::endl;
               }
               else
                   std::cout << RED << "--- accept failed" << RESET << std::endl;
           }
           else
               std::cout << "-- no new connection" << std::endl;
           
           // CLIENT MESSAGE SECTION
           //std::cout << BLUE << "- CLIENT MESSAGE CHECK" << RESET << std::endl;
           for (size_t i = 0; i < _clients.size(); ++i)
           {
               //std::cout << "-- checking client " << i << " fd = " << _clients[i]->getFd() << std::endl;
               if (FD_ISSET(_clients[i]->getFd(), &readfds))
               {
                   //std::cout << PINK << "--- client " << _clients[i]->getFd() << " has data" << RESET << std::endl;
                   
                   // RECV SECTION
                   //std::cout << "- RECV DATA" << std::endl;
                   char buffer[512];
                   int bytesReceived = recv(_clients[i]->getFd(), buffer, sizeof(buffer) - 1, 0);
                   //std::cout << "-- recv returned = " << bytesReceived << " bytes" << std::endl;
                   
                   if (bytesReceived > 0)
                   {
                       buffer[bytesReceived] = '\0';
                       //std::cout << PINK << "--- message content = " << buffer << RESET << std::endl;
                       
                       // NOUVELLE SECTION avec checkEnd
                       //std::cout << "- BUFFER PROCESSING" << std::endl;
                       _clients[i]->addData(std::string(buffer));
                       
                       //std::cout << "- MESSAGE EXTRACTION" << std::endl;
                       while (_clients[i]->checkEnd())
                       {
                           std::string completeMessage = _clients[i]->getCommand();
                           //std::cout << "-- processing complete message" << std::endl;
                           parseMessage(completeMessage);
                       }
                       //std::cout << "--- message extraction done" << std::endl;
                       
                       // SEND SECTION
                       //std::cout << "- SEND RESPONSE" << std::endl;
                       std::string response = "SERVER = ";
                       response += buffer;
                       response += "\r\n";
                       
                       //std::cout << "-- sending to client " << _clients[i]->getFd() << std::endl;
                       //int sentBytes = send(_clients[i]->getFd(), response.c_str(), response.length(), 0);
                       //std::cout << GREEN << "--- sent " << sentBytes << " bytes" << RESET << std::endl;
                   }
                   else if (bytesReceived == 0)
                   {
                       // DISCONNECT SECTION
                       //std::cout << YELLOW << "- CLIENT DISCONNECT" << RESET << std::endl;
                       //std::cout << "-- client " << _clients[i]->getFd() << " disconnected cleanly" << std::endl;
                       close(_clients[i]->getFd());
                       delete (_clients[i]);
                       //std::cout << "--- client object deleted" << std::endl;
                       
                       _clients.erase(_clients.begin() + i);
                       //std::cout << "---- remaining clients = " << _clients.size() << std::endl;
                       break;
                   }
                   else
                   {
                       // ERROR SECTION
                       //std::cout << RED << "- CLIENT ERROR" << RESET << std::endl;
                       //std::cout << "-- recv error for client " << _clients[i]->getFd() << std::endl;
                       close(_clients[i]->getFd());
                       delete _clients[i];
                       _clients.erase(_clients.begin() + i);
                       std::cout << "--- client removed due to error. remaining = " << _clients.size() << std::endl;
                       break;
                   }
               }
               else
                   std::cout << "--- client " << _clients[i]->getFd() << " has no data" << std::endl;
           }
           //std::cout << "-- client check complete" << std::endl;
        }
        else
           std::cout << RED << "- SELECT ERROR" << RESET << std::endl;      
        
        std::cout << GREEN << "**** LOOP TURN OK" << std::endl;
        std::cout << "************" << RESET << std::endl;
   }
}