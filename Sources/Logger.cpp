#include "../Includes/Logger.hpp"

size_t Logger::indent = 0;

void Logger::log(std::string message){
    std::string s;
    s.insert(s.begin(), indent - s.size(), ' ');

    std::cout << s << message << "\n";
}
void Logger::err(std::string message){
    std::cout << "Error: " << message << "\n";
}
