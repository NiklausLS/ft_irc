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

bool Client::checkEnd() const
{
    std::cout << "-- check if \\r\\n buffer" << std::endl;
    
    size_t check = _inputBuffer.find("\r\n");
    if (check != std::string::npos)
    {
        std::cout << "-- checkEnd = OK" << std::endl;
        return (true);
    }
    else
    {
        std::cout << "-- checkEnd = KO" << std::endl;
        return (false);
    }
}

std::string Client::getCommand()
{
    std::cout << "- START GETCOMMAND" << std::endl;
    
    size_t pos = _inputBuffer.find("\r\n");
    if (pos == std::string::npos)
    {
        std::cout << "-- no \\r\\n found" << std::endl;
        return ("");
    }

    std::string message = _inputBuffer.substr(0, pos);
    _inputBuffer = _inputBuffer.substr(pos + 2);
    
    std::cout << "-- extracting " << message << std::endl;
    std::cout << "--- rest of buffer = " << _inputBuffer << std::endl;
    return (message);
}