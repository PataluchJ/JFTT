#ifndef VALUE_HPP
#define VALUE_HPP

#include "Nodes.hpp"

/*
Value: 
    | num -> ConstValue
    | var -> VarValue
*/

class ConstValue : public Value{
public:
    NumberType value;
    ConstValue(NumberType value);
    ~ConstValue();
    InstructionList* valueToRegister(Register r) override;
    bool isConst() override;
};

class VarValue : public Value{
public:
    Identifier* id;
    VarValue(Identifier* iden);
    ~VarValue();
    InstructionList* valueToRegister(Register r) override;
    bool isConst() override;
};

#endif