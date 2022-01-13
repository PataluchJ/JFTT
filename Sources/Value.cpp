#include "Value.hpp"

ConstValue::ConstValue(NumberType value){
    this->value = value;
}

VarValue::VarValue(Identifier* iden){
    this->id = iden;
}

ConstValue::~ConstValue(){

}

VarValue::~VarValue(){
    delete id;
}

InstructionList* VarValue::valueToRegister(Register r){
    auto inst = id->addressToRegister(r);
    inst->push_back(new Instruction(OptCode::LOAD, r));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    return inst;
}

InstructionList* ConstValue::valueToRegister(Register r){
    auto inst = generateNumber(value,r);
    return inst;
}

bool ConstValue::isConst() { return true; };
bool VarValue::isConst() { return false; }