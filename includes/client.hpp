#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include "check.hpp"

class Client
{
private:
    int _fd;
    std::string _inputBuffer;

public:
    Client();
    ~Client();
    Client(int fd);
    Client(const Client& copy);
    Client& operator=(const Client& copy);
    
    int getFd() const;
    void addData(const std::string& data);
};

#endif