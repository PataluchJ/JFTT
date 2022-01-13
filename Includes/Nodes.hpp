#ifndef NODES_HPP
#define NODES_HPP

#include <Environment.hpp>

/*

Identifier : 
    | var -> Variable
    | arr[var] -> ArrayByVariable
    | arr[const] -> ArrayByConst

Value: 
    | num -> ConstValue
    | var -> IDentifier

Condition:
    | eq -> ConditionEq
    | neq -> ConditionNeq 
    ...

Expression:
    | value -> ConstExpression 
    | v Plus v -> AddExpression
    | ...

Command:
    | Assign, If, IfElse, While, Repeat, For, Read, Write

Commands: 
    list od command
*/


class Node{
public:
    size_t definedAtLine;
};

class Identifier : public Node{
public:
    StringType name;
    virtual InstructionList* addressToRegister(Register r) = 0;
};

class Value : public Node{
public:
    virtual bool isConst() = 0;
    virtual InstructionList* valueToRegister(Register r) = 0;
};

class Expression : public Node {
public:
    virtual InstructionList* calculateToRegister(Register r) = 0;
};

#endif