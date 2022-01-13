#include "../Assembly.hpp"

#include <iostream>

int main(){
    std::cout << "Assembly test.\n";
    InstructionList* list = new InstructionList();
    list->push_back(new Instruction(OptCode::GET));
    list->push_back(new Instruction(OptCode::PUT));
    list->push_back(new Instruction(OptCode::LOAD, Register::a));
    list->push_back(new Instruction(OptCode::STORE, Register::b));
    list->push_back(new Instruction(OptCode::ADD, Register::c));
    list->push_back(new Instruction(OptCode::SUB, Register::d));
    list->push_back(new Instruction(OptCode::SHIFT, Register::e));
    list->push_back(new Instruction(OptCode::RESET, Register::f));
    list->push_back(new Instruction(OptCode::INC, Register::g));
    list->push_back(new Instruction(OptCode::DEC, Register::a));
    list->push_back(new Instruction(OptCode::JUMP, 69));
    list->push_back(new Instruction(OptCode::JPOS, 69));
    list->push_back(new Instruction(OptCode::JZERO, 69));
    list->push_back(new Instruction(OptCode::JNEG, 69));
    list->push_back(new Instruction(OptCode::HALT));
    std::cout << "All asm instruction pushed to instruction list.\nPrinting list...\n";
    for(auto i : *list){
        std::cout << i->toString()  << "\n";
    }
    std::cout << "Test end.\n";

    std::cout << "Testing generatiung number: 1001110000110101010\n";
    long long n = 21;
    auto num = generateNumber(n, Register::a);
    for(auto i : *num){
        std::cout << i->toString() << "\n";
    }
    std::cout << "Testing neg number -21:\n";
    long long p = -21;
    auto n2 = generateNumber(p, Register::b);
    for(auto i : *n2){
        std::cout << i->toString() << "\n";
    }

    return 0;
}