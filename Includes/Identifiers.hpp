#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include "Nodes.hpp"
#include "Value.hpp"

/*
Identifier : 
    | var -> Variable
    | arr[var] -> ArrayByVariable
    | arr[const] -> ArrayByConst

*/
class Variable : public Identifier {
public: 
    Variable(StringType name, int line);
    ~Variable();
    bool isHelper;
    InstructionList* addressToRegister(Register r) override;
    bool isIterator() override;
    bool isInit() override;
};
class ArrayByVariable : public Identifier {
public: 
    StringType index;
    ArrayByVariable(StringType name, StringType index, int line);
    ~ArrayByVariable();
    InstructionList* addressToRegister(Register r) override;
    bool isIterator() override;
    bool isInit() override;
};

class ArrayByConst : public Identifier {
public: 
    NumberType index;
    ArrayByConst(StringType name, NumberType index, int line);
    ~ArrayByConst();
    InstructionList* addressToRegister(Register r) override;
    bool isIterator() override;
    bool isInit() override;
};


#endif