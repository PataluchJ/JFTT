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
    InstructionList* inst = new InstructionList;
    //Logger::log("Generating " + std::to_string(num));
    auto signedINC = OptCode::INC;
    if(num < 0){
        signedINC = OptCode::DEC;
        num *= -1;
    }

    if(num <= 7 && num >= -7){
        inst->push_back(new Instruction(OptCode::RESET, r));
        while(num > 0){
            num--;
            inst->push_back(new Instruction(signedINC, r));
        }
        //Logger::log("generated short...");
        return inst;
    }

    auto temp = r;
    if(r == Register::a)
        temp = Register::b;

    inst->push_back(new Instruction(OptCode::RESET, temp));
    inst->push_back(new Instruction(OptCode::INC, temp));
    inst->push_back(new Instruction(OptCode::INC, temp));
    inst->push_back(new Instruction(OptCode::INC, temp));

    NumberType mask = 0b111;
    int helper[64/3+1];
    int hIndex = 0;

    while(num > 0){
        helper[hIndex] = num&mask;
        hIndex++;
        num >>= 3;
    }
    hIndex--;
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    while(hIndex > -1){
        while(helper[hIndex] > 0){
            inst->push_back(new Instruction(signedINC, Register::a));
            helper[hIndex]--;
        }
        hIndex--;
        inst->push_back(new Instruction(OptCode::SHIFT, temp));
    }
    //Logger::log("generated long...");
    inst->pop_back();
    if(r != Register::a){
        inst->push_back(new Instruction(OptCode::SWAP, r));
    }
    return inst;
}
void deleteInstructionList(InstructionList* list){
    for(auto i : *list){
        delete i;
    }
    delete list;
}