#include "../Includes/Identifiers.hpp"

Variable::Variable(StringType name){
    this->name = name;
}

ArrayByVariable::ArrayByVariable(StringType name, StringType index){
    this->name = name;
    this->index = index;
}

ArrayByConst::ArrayByConst(StringType name, NumberType index){
    this->name = name;
    this->index = index;
}
bool Variable::isIterator(){
    auto mem = Environment::getVariable(*name);
    return (mem->type == MemVar::MemVarType::Iterator);
}
bool ArrayByVariable::isIterator(){
    auto mem = Environment::getVariable(*name);
    return (mem->type == MemVar::MemVarType::Iterator);
}
bool ArrayByConst::isIterator(){
    auto mem = Environment::getVariable(*name);
    return (mem->type == MemVar::MemVarType::Iterator);
}
Variable::~Variable(){
    delete name;
}
ArrayByVariable::~ArrayByVariable(){
    delete name;
    delete index;
}
ArrayByConst::~ArrayByConst(){
    delete name;
}

InstructionList* Variable::addressToRegister(Register r){
    auto memVar = Environment::getVariable(*name);
    NumberType address = (NumberType)(memVar->address);
    auto inst = generateNumber(address, r);
    return inst;
}

InstructionList* ArrayByVariable::addressToRegister(Register r){
    /*
        Get address of cel inside of array indexed by variable
    */
    //Logge::log("Array by variable");
    std::string* indexNameCopy = new std::string(*(this->index));
    std::string* nameCopy = new std::string(*(this->name));

    Value* indexValue = new VarValue(new Variable(indexNameCopy));
    Identifier* array = new Variable(nameCopy);

    Register temp = r;
    if(temp == Register::b || temp == Register::a)
        temp = Register::c;

    auto arrayZeroAddressInst = array->addressToRegister(temp);
    auto indexValueInst = indexValue->valueToRegister(Register::a);

    auto inst = new InstructionList;
    inst->splice(inst->end(), *arrayZeroAddressInst);
    inst->splice(inst->end(), *indexValueInst);
    inst->push_back(new Instruction(OptCode::ADD, temp));
    inst->push_back(new Instruction(OptCode::SWAP, r));

    delete arrayZeroAddressInst;
    delete indexValueInst;

    //delete array;
    //delete indexValueInst;

    return inst;
}

InstructionList* ArrayByConst::addressToRegister(Register r){
    /*
        Get address of cell inside array indexed by const value
    */
    auto arrayMem = Environment::getVariable(*name);
    NumberType address = this->index + arrayMem->address;

    auto inst = generateNumber(address, r);
    return inst;
}

