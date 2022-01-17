#include "../Includes/Assembly.hpp"

//namespace Machine {
    const OptCode* OptCode::GET    = new OptCode("GET", OptCode::OptCodeType::Get, OptCode::ArgumentType::None);
    const OptCode* OptCode::PUT    = new OptCode("PUT", OptCode::OptCodeType::Put, OptCode::ArgumentType::None);
    const OptCode* OptCode::LOAD   = new OptCode("LOAD", OptCode::OptCodeType::Load, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::STORE  = new OptCode("STORE", OptCode::OptCodeType::Store, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::ADD    = new OptCode("ADD", OptCode::OptCodeType::Add, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::SUB    = new OptCode("SUB", OptCode::OptCodeType::Sub, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::SHIFT  = new OptCode("SHIFT", OptCode::OptCodeType::Shift, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::SWAP   = new OptCode("SWAP", OptCode::OptCodeType::Swap, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::RESET  = new OptCode("RESET", OptCode::OptCodeType::Reset, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::INC    = new OptCode("INC", OptCode::OptCodeType::Inc, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::DEC    = new OptCode("DEC", OptCode::OptCodeType::Dec, OptCode::ArgumentType::Reg);
    const OptCode* OptCode::JUMP   = new OptCode("JUMP", OptCode::OptCodeType::Jump, OptCode::ArgumentType::ConstValue);
    const OptCode* OptCode::JPOS   = new OptCode("JPOS", OptCode::OptCodeType::Jpos, OptCode::ArgumentType::ConstValue);
    const OptCode* OptCode::JZERO  = new OptCode("JZERO", OptCode::OptCodeType::Jzero, OptCode::ArgumentType::ConstValue);
    const OptCode* OptCode::JNEG   = new OptCode("JNEG", OptCode::OptCodeType::Jneg, OptCode::ArgumentType::ConstValue);
    const OptCode* OptCode::HALT   = new OptCode("HALT", OptCode::OptCodeType::Halt, OptCode::ArgumentType::None);
    OptCode::OptCode(std::string name, OptCode::OptCodeType type, OptCode::ArgumentType argType) 
    : name(name), codeType(type), argType(argType) {}

//}


//Instruction::Instruction(Machine::OptCode* code, Machine::Register reg) {
Instruction::Instruction(const OptCode* code, Register reg) : code(code) {
    //this->code = code;
    argument.reg = reg;
}
//Instruction::Instruction(Machine::OptCode* code, NumberType num) {
Instruction::Instruction(const OptCode* code, NumberType num) : code(code) {
    //this->code = code;
    argument.num = num;
}
//Instruction::Instruction(Machine::OptCode* code) {
Instruction::Instruction(const OptCode* code) : code(code) {
    //this->code = code;
}

std::string Instruction::toString(){
    std::string ret = this->code->name;
    if(this->code->argType == OptCode::ArgumentType::ConstValue){
        ret += " " + std::to_string(this->argument.num);
    }
    else if(this->code->argType == OptCode::ArgumentType::Reg){
        char r = char(int('a') + this->argument.reg);
        ret += " ";
        ret.push_back(r);
    }
    
    return ret;
}

InstructionList* generateNumber(NumberType num, Register r){
    //Logger::log("Generating number " + std::to_string(int(num)));
    InstructionList* inst = new InstructionList;
    Register temp = Register::b;
    if(r != Register::a)
        temp = r;
    // Put 1 in temp register 
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::RESET, temp));
    inst->push_back(new Instruction(OptCode::INC, temp));

    char* helper = new char[sizeof(NumberType)*8+1];
    int helper_index = 0;
    bool neg = num < 0;
    if(neg) {
        num *= -1;
    }
    while(num > 0){
        helper[helper_index] = num & 1;
        num >>= 1;
        helper_index++;
    }
    helper_index--;
    while(helper_index >= 0) {
        if(helper[helper_index] == 1){
            if(neg)
                inst->push_back(new Instruction(OptCode::DEC, Register::a));
            else
                inst->push_back(new Instruction(OptCode::INC, Register::a));
        }
        inst->push_back(new Instruction(OptCode::SHIFT,temp));
        helper_index--;
    }
    inst->pop_back();
    
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));
    
    delete[] helper;
    return inst;
}

void deleteInstructionList(InstructionList* list){
    for(auto i : *list){
        delete i;
    }
    delete list;
}