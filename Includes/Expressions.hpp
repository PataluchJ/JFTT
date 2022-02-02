#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include "Nodes.hpp"
#include "Value.hpp"
#include <math.h>

class ConstExpression : public Expression {
public:
    Value* val;
    ConstExpression(Value* val);
    ~ConstExpression();
    InstructionList* calculateToRegister(Register r) override;
};

class AddExpression : public Expression {
public:
    Value* left;
    Value* right;
    AddExpression(Value* left, Value* right);
    ~AddExpression();
    InstructionList* calculateToRegister(Register r) override;
};

class SubExpression : public Expression {
public:
    Value* left;
    Value* right;
    SubExpression(Value* left, Value* right);
    ~SubExpression();
    InstructionList* calculateToRegister(Register r) override;
};

class MulExpression : public Expression {
public:
    Value* left;
    Value* right;
    MulExpression(Value* left, Value* right);
    ~MulExpression();
    InstructionList* calculateToRegister(Register r) override;
    InstructionList* generateForBothConst(Register r);
    InstructionList* generateForOneConst(Register r);
    InstructionList* generateForNoConst(Register r);
};

class DivExpression : public Expression {
public:
    Value* left;
    Value* right;
    DivExpression(Value* left, Value* right);
    ~DivExpression();
    InstructionList* calculateToRegister(Register r) override;
    InstructionList* generateForBothConst(Register r);
    //InstructionList* generateForOneConst(Register r);
    InstructionList* generateForNoConst(Register r);
};

class ModExpression : public Expression {
public:
    Value* left;
    Value* right;
    ModExpression(Value* left, Value* right);
    ~ModExpression();
    InstructionList* calculateToRegister(Register r) override;
    InstructionList* generateForBothConst(Register r);
    InstructionList* generateForTwoPow(Register r);
    InstructionList* generateForNoConst(Register r);
};

#endif