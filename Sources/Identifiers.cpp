#include "../Includes/Identifiers.hpp"

Variable::Variable(StringType name, int line){
    this->name = name;
    this->definedAtLine = line;
    this->isHelper = false;
}

ArrayByVariable::ArrayByVariable(StringType name, StringType index, int line){
    this->name = name;
    this->index = index;
    this->definedAtLine = line;
}

ArrayByConst::ArrayByConst(StringType name, NumberType index, int line){
    this->name = name;
    this->index = index;
    this->definedAtLine = line;
}
bool Variable::isInit(){
    auto mem = Environment::getVariable(*name);
    return mem->initialized;
}
bool ArrayByVariable::isInit(){
    return true;
}
bool ArrayByConst::isInit(){
    return true;
}
bool Variable::isIterator(){
    auto mem = Environment::getVariable(*name);
    if(mem == Environment::undef){
        Logger::err("Undefined variable \"" + *name + "\" at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    return (mem->type == MemVar::MemVarType::Iterator);
}
bool ArrayByVariable::isIterator(){
    auto mem = Environment::getVariable(*name);
    if(mem == Environment::undef){
        Logger::err("Undefined variable \"" + *name + "\" at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    return (mem->type == MemVar::MemVarType::Iterator);
}
bool ArrayByConst::isIterator(){
    auto mem = Environment::getVariable(*name);
    if(mem == Environment::undef){
        Logger::err("Undefined variable \"" + *name + "\" at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
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
    if(memVar->type == MemVar::MemVarType::Array && !isHelper){
        Logger::err("Using array \"" + *name + "\" as variable at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    if(memVar == Environment::undef){
        Logger::err("Undefined variable \"" + *name + "\" at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    if(memVar->type == MemVar::MemVarType::Iterator){
        bool inScope = false;
        for(auto i : Environment::iteratorScope){
            if(i == *name){
                inScope = true;
            }
        }
        if(!inScope){
            Logger::err("Undefined variable \"" + *name + "\" at line "  + std::to_string(this->definedAtLine));
            Environment::declareError();
        }
    }
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

    Value* indexValue = new VarValue(new Variable(indexNameCopy, this->definedAtLine));
    Variable* array = new Variable(nameCopy, this->definedAtLine);
    array->isHelper = true;
    
    auto memVar = Environment::getVariable(*nameCopy);
    if(memVar->type != MemVar::MemVarType::Array){
        Logger::err("Using variable \"" + *nameCopy + "\" as array at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    
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
    if(arrayMem->type != MemVar::MemVarType::Array){
        Logger::err("Using variable \"" + *name + "\" as array at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    if(arrayMem == Environment::undef){
        Logger::err("Undefined variable \"" + *name + "\" at line " + std::to_string(this->definedAtLine));
        Environment::declareError();
    }
    NumberType address = this->index + arrayMem->address;

    auto inst = generateNumber(address, r);
    return inst;
}

