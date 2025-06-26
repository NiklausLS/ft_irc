#include "../includes/client.hpp"

Client::Client() : _fd(-1)
{
    //std::cout << YELLOW << "CLIENT CONSTRUCTOR" << RESET << std::endl;
}

Client::~Client()
{
    //std::cout << YELLOW << "CLIENT DESTRUCTOR FD = " << _fd << RESET << std::endl;
}

Client::Client(int fd) : _fd(fd)
{
    //std::cout << BLUE << "-- CLIENT CONSTRUCTOR FD = " << _fd << RESET << std::endl;
}

Client::Client(const Client& copy)
{
    _fd = copy._fd;
    _inputBuffer = copy._inputBuffer;
}

Client& Client::operator=(const Client& copy)
{
    if (this != &copy)
    {
        _fd = copy._fd;
        _inputBuffer = copy._inputBuffer;
    }
    return (*this);
}

int Client::getFd() const
{
    return (_fd);
}

void Client::addData(const std::string& data)
{
    _inputBuffer += data;
    std::cout << PINK << "buffer = " << _inputBuffer << RESET << std::endl;
}
