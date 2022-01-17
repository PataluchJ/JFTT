#include "../Includes/Logger.hpp"

size_t Logger::indent = 0;

void Logger::log(std::string message){
    std::cout << message << "\n";
}
void Logger::err(std::string message){
    std::cout << "Error: " << message << "\n";
}
