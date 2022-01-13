#include "Expressions.hpp"

ConstExpression::ConstExpression(Value* value){
    this->val = value;
}
AddExpression::AddExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
SubExpression::SubExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
MulExpression::MulExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
DivExpression::DivExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
ModExpression::ModExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}

ConstExpression::~ConstExpression(){
    delete val;
}
AddExpression::~AddExpression(){
    delete left;
    delete right;
}
SubExpression::~SubExpression(){
    delete left;
    delete right;
}
MulExpression::~MulExpression(){
    delete left;
    delete right;
}
DivExpression::~DivExpression(){
    delete left;
    delete right;
}
ModExpression::~ModExpression(){
    delete left;
    delete right;
}

InstructionList* ConstExpression::calculateToRegister(Register r){
    auto inst = this->val->valueToRegister(r);
    return inst;
}
InstructionList* AddExpression::calculateToRegister(Register r){
    if(this->left->isConst() && this->right->isConst()){
        NumberType value = ((ConstValue*)left)->value + ((ConstValue*)right)->value;
        return generateNumber(value, r);
    }
    else if(this->left->isConst() || this->right->isConst()){
        Value* constant = left->isConst() ? left : right;
        Value* variable = left->isConst() ? right : left;
        if(((ConstValue*)constant)->value == 1){
            auto inst = variable->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::INC, r));
            return inst;
        }
        else if (((ConstValue*)constant)->value == -1){
            auto inst = variable->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::DEC, r));
            return inst;
        }
    }
    auto inst = this->right->valueToRegister(Register::c);
    auto loadLeftToA = this->left->valueToRegister(Register::a);
    inst->merge(*loadLeftToA);
    inst->push_back(new Instruction(OptCode::ADD, Register::c));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete loadLeftToA;
    return inst;
}
InstructionList* SubExpression::calculateToRegister(Register r){
    if(this->left->isConst() && this->right->isConst()){
        NumberType value = ((ConstValue*)left)->value - ((ConstValue*)right)->value;
        return generateNumber(value, r);
    }
    else if(this->right->isConst()){
        if(((ConstValue*)right)->value == 1){
            auto inst = this->left->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::DEC, r));
            return inst;
        }
        else if(((ConstValue*)right)->value == -1){
            auto inst = this->left->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::INC, r));
            return inst;
        }
    }
    auto inst = this->right->valueToRegister(Register::c);
    auto loadLeftToA = this->left->valueToRegister(Register::a);
    inst->merge(*loadLeftToA);
    inst->push_back(new Instruction(OptCode::SUB, Register::c));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete loadLeftToA;
    return inst;
}
InstructionList* MulExpression::calculateToRegister(Register r){
    InstructionList* inst = new InstructionList;
    return inst;
}
InstructionList* DivExpression::calculateToRegister(Register r){
    InstructionList* inst = new InstructionList;
    return inst;
}
InstructionList* ModExpression::calculateToRegister(Register r){
    InstructionList* inst = new InstructionList;
    return inst;
}