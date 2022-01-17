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
        arrat[var]->address = array->address + (index.val - leftBound)
        Load LeftBound into c
        Load index value to a
        Sub c
        Load address to c
        Add c
        Profit
    */
    auto arrayMem = Environment::getVariable(*name);
    auto indexMem = Environment::getVariable(*index);

    Register temp = Register::b;
    if(r != Register::a)
        temp = r;

    // Load left bound to c
    auto inst = generateNumber(arrayMem->leftBound, temp);
    auto indexAddressInst = generateNumber(indexMem->address, Register::a);
    auto arrayAddressInst = generateNumber(arrayMem->address, temp);
    // Load index value to a
   // inst->merge(*indexAddressInst);
    inst->splice(inst->end(), *indexAddressInst);
    inst->push_back(new Instruction(OptCode::LOAD, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, temp));
    //inst->merge(*arrayAddressInst);
    inst->splice(inst->end(), *arrayAddressInst);
    inst->push_back(new Instruction(OptCode::ADD, temp));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    //delete indexAddressInst;
    //delete arrayAddressInst;
    return inst;
}

InstructionList* ArrayByConst::addressToRegister(Register r){
    /*
        array[const]->address = array->address + (index - leftBound)
        ... = array->address + offset
    */
    auto arrayMem = Environment::getVariable(*name);

    Register temp = Register::b;
    if(r != Register::a)
        temp = r;

    NumberType offset = index - arrayMem->leftBound;
    auto inst = generateNumber(offset, Register::a);
    auto arrayAddressInst = generateNumber(arrayMem->address, temp);
    //inst->merge(*arrayAddressInst);
    inst->splice(inst->end(), *arrayAddressInst);
    inst->push_back(new Instruction(OptCode::ADD, temp));

    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete arrayAddressInst;
    return inst;
}

