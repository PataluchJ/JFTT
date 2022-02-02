#include "../Includes/Value.hpp"

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

bool ConstValue::isInit(){
    return true;
}
bool VarValue::isInit(){
    return this->id->isInit();
}

InstructionList* VarValue::valueToRegister(Register r){
    //Logger::log("VarValue{");
    Logger::indent += 1;
    auto inst = id->addressToRegister(r);
    inst->push_back(new Instruction(OptCode::LOAD, r));
    inst->push_back(new Instruction(OptCode::SWAP, r));

    Logger::indent -= 1;
    //Logger::log("}VarValue");

    return inst;
}

InstructionList* ConstValue::valueToRegister(Register r){
    //Logger::log("ConstValue{");
    Logger::indent += 1;
    auto inst = generateNumber(value,r);
    
    Logger::indent -= 1;
    //Logger::log("}ConstValue");
    
    return inst;
}

bool ConstValue::isConst() { return true; };
bool VarValue::isConst() { return false; }