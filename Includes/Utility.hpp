#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <fstream>
#include "Environment.hpp"

class Util {
public:
    static void printMemVar(MemVar* var);
    static void printList(InstructionList* list);
    static void saveASM(InstructionList* list, std::string file);
};

#endif