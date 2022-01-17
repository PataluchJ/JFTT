#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <list>
#include <string>
#include "Logger.hpp"

typedef long long NumberType;
typedef std::string* StringType;

enum Register {
    a = 0,
    b = 1,
    c = 2,
    d = 3,
    e = 4,
    f = 5,
    g = 6,
    h = 7,
    None
};

struct OptCode{
    enum OptCodeType{
        Get,
        Put,
        Load,
        Store,
        Add,
        Sub,
        Shift,
        Swap,
        Reset,
        Inc,
        Dec,
        Jump,
        Jpos,
        Jzero,
        Jneg,
        Halt,
        Error
    };
    enum class ArgumentType : int{
        None,
        Reg,
        ConstValue
    };

    std::string name;
    OptCodeType codeType;
    ArgumentType argType;

    OptCode(std::string name, OptCodeType type, ArgumentType argType);

    static const OptCode *GET, *PUT, *LOAD, *STORE, *ADD, *SUB, *SHIFT, *SWAP, *RESET, *INC, *DEC, *JUMP, *JPOS, *JZERO, *JNEG, *HALT;

};

struct Instruction{
    const OptCode* code;
    union _argument{
        Register reg;
        NumberType num;
    } argument;
    
    Instruction(const OptCode* code, Register reg);
    Instruction(const OptCode* code, NumberType cons);
    Instruction(const OptCode* code);

    std::string toString();
};

typedef std::list<Instruction*> InstructionList;

InstructionList* generateNumber(NumberType num, Register r);

void deleteInstructionList(InstructionList* list);

#endif