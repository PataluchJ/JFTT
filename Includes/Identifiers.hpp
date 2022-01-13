#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <Nodes.hpp>

/*
Identifier : 
    | var -> Variable
    | arr[var] -> ArrayByVariable
    | arr[const] -> ArrayByConst

*/
class Variable : public Identifier {
public: 
    Variable(StringType name);
    ~Variable();
    InstructionList* addressToRegister(Register r) override;
};
class ArrayByVariable : public Identifier {
public: 
    StringType index;
    ArrayByVariable(StringType name, StringType index);
    ~ArrayByVariable();
    InstructionList* addressToRegister(Register r) override;
};

class ArrayByConst : public Identifier {
public: 
    NumberType index;
    ArrayByConst(StringType name, NumberType index);
    ~ArrayByConst();
    InstructionList* addressToRegister(Register r) override;
};


#endif