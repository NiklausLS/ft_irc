#include <iostream>
#include <string>
#include "check.hpp"
#include "server.hpp"

//to do list
//check args nbr
//check le password
//check le port
//
int main(int argc, char **argv)
{

    Data parsing = Check::checkAll(argc, argv);
    //std::cout << "--- MAIN ---" << std::endl;
    //std::cout << "port = " << parsing.port << " | " << "password = " << parsing.password << std::endl;


    Server irc(parsing.port, parsing.password);
    irc.exec();

    return (0);
}