#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

class Logger{
public:
    static void log(std::string message);
    static void err(std::string message);
    static size_t indent;
};

#endif