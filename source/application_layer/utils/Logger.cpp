#include "../../game_logic/game_app/GameApp.h"
#include "Logger.h"
#include <iostream>

using namespace Application_Layer;

void Logger::Log(Logger::LOG_FLAG logFlag, const string& message)
{
    switch(logFlag)
    {
        case INFO:
                std::cout << "[INFO] " << message << std::endl;
            break;
            
        case ERROR:
                std::cout << "[ERROR] " << message << std::endl;
            break;
            
        case WARNING:
                std::cout << "[WARNING] "  << message << std::endl;
            break;
    }
}