#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Assembly.hpp"

#include <map>
#include <vector>

struct MemVar{
    bool initialized;
    bool error;
    size_t address;
    NumberType leftBound;
    NumberType rightBound;
    MemVar();
    MemVar(bool init, bool err, size_t addres);
};

class Environment {
    static std::map<std::string, MemVar*> variables;
    static std::vector<Instruction*> labels;
    static size_t nextFree;
    static size_t nextLabel;
    static MemVar* undef;
public:
    static size_t reserve(size_t size);
    static void declareVariable(std::string name);
    static void declareArray(std::string name, NumberType leftBound, NumberType rightBoudn);
    static MemVar* getVariable(std::string name);

    static size_t laberInstruction(Instruction* inst);
    static Instruction* getLabeledInstruction(size_t label);
};

#endif