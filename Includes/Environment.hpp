#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Assembly.hpp"

#include <map>
#include <vector>

struct MemVar{
    enum MemVarType{
        Normal,
        Iterator,
        Array
    } type;
    bool initialized;
    bool error;
    size_t address;
    NumberType leftBound;
    NumberType rightBound;
    MemVar();
    MemVar(bool init, bool err, size_t addres);
    MemVar(size_t address, MemVarType type);
    MemVar(size_t address, NumberType left, NumberType right);
};

class Environment {
    static std::map<std::string, MemVar*> variables;
    static std::vector<Instruction*> labels;
    static size_t nextFree;
    static size_t nextLabel;
    static MemVar* undef;
    struct ArrayDeclaration{
        std::string name;
        NumberType leftBound;
        NumberType rightBound;
        ArrayDeclaration(std::string name, NumberType left, NumberType right);
    };
    static std::list<std::string> variableDeclarations;
    static std::list<std::string> iteratorDeclarations;
    
    static std::list<ArrayDeclaration> arraysDeclarations;
    
    static void addToMemory(std::string name, MemVar* var);

    static bool errorDeclared;
public:
    static bool declarationsFailed;

    static size_t reserve(size_t size);
    static void declareVariable(std::string name);
    static void declareIterator(std::string name);
    static void declareArray(std::string name, NumberType leftBound, NumberType rightBoudn);
    static void finalizeDeclarations();
    static MemVar* getVariable(std::string name);

    static size_t laberInstruction(Instruction* inst);
    static Instruction* getLabeledInstruction(size_t label);

    static void declareError();
    static bool compilationFailed();
};

#endif