#include "../includes/check.hpp"
#include "../includes/message.hpp"
#include <iostream>

Message::Message()
{
    _command = "";
}

Message::Message(const Message& other)
{
    _command = other._command;
    _params = other._params;
}

Message& Message::operator=(const Message& other)
{
    if (this != &other)
    {
        _command = other._command;
        _params = other._params;
    }
    return (*this);
}

Message::~Message()
{}

//PARSE COMMAND 
bool Message::parseSimple(const std::string& input)
{
    std::cout << "--- PARSING: " << input << " ---" << std::endl;
    
    if (input.empty())
    {
        std::cout << RED << "EMPTY INPUT" << RESET << std::endl;
        return (false);
    }
    
    //CLEAR OLD DATA
    _command = "";
    _params.clear();
    
    //JUST GET THE FIRST WORD AS COMMAND
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos)
    {
        _command = input.substr(0, spacePos);
        std::cout << GREEN << "COMMAND = " << _command << RESET << std::endl;
        return (true);
    }
    else
    {
        _command = input;
        std::cout << GREEN << "COMMAND = " << _command << RESET << std::endl;
        return (true);
    }
}

//RETURN COMMAND STRING
std::string Message::getCommand() const
{
    return _command;
}

//RETURN NUMBER OF PARAMETERS
int Message::getParamCount() const
{
    return _params.size();
}

//DISPLAY MESSAGE INFO FOR DEBUG
void Message::display() const
{
    std::cout << BLUE << "COMMAND: " << _command << RESET << std::endl;
    std::cout << BLUE << "PARAMS: " << _params.size() << RESET << std::endl;
}