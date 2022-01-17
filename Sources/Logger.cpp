#include "../Includes/Logger.hpp"

void Logger::log(std::string message){
    std::cout << message << "\n";
}
void Logger::err(std::string message){
    std::cout << "Error: " << message << "\n";
}
