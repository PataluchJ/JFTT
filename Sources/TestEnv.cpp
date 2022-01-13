#include "../Environment.hpp"

void printMemVar(MemVar* var){
    std::cout << "Adr: " << var->address << " Init: " << var->initialized << " Err: " << var->error << "\n";
}

void printList(InstructionList* list){
    for(auto l : *list){
        std::cout << l->toString() << "\n";
    }
}

int main(){
    std::cout << "Testing variable definitions\n";
    std::cout << "Declaring variables n, array[0:2], p\n";
    Environment::declareVariable("n");
    Environment::declareArray("array", -2, 3);
    Environment::declareVariable("p");
    std::cout << "n:\n";
    printMemVar(Environment::getVariable("n"));
    std::cout << "array:\n";
    printMemVar(Environment::getVariable("array"));
    std::cout << "p:\n";
    printMemVar(Environment::getVariable("p"));
    std::cout << "Trying to access not existend variable:\n";
    printMemVar(Environment::getVariable("none"));
    std::cout << "Redefining p\n";
    Environment::declareVariable("p");
    printMemVar(Environment::getVariable("p"));
    std::cout << "Defining array with wrong indexes:\n";
    Environment::declareArray("wrong", 20,10);
    printMemVar(Environment::getVariable("wrong"));
    
    std::cout << "Testing labels\n";
    InstructionList* l1 = new InstructionList;
    InstructionList* l2 = new InstructionList;


    auto labeled1 = new Instruction(OptCode::LOAD, Register::b);
    auto labeled2 = new Instruction(OptCode::STORE, Register::b);

    l1->push_back(new Instruction(OptCode::GET));
    l1->push_back(labeled1);
    l1->push_back(new Instruction(OptCode::INC, Register::b));
    l2->push_back(new Instruction(OptCode::PUT));
    l2->push_back(labeled2);
    l2->push_back(new Instruction(OptCode::HALT));

    size_t lab1 = Environment::laberInstruction(labeled1);
    size_t lab2 = Environment::laberInstruction(labeled2);

    InstructionList* inst = new InstructionList;
    inst->merge(*l1);
    inst->merge(*l2);
std::cout << "INSERT\n";
    std::cout << "Not modyfied instruction list:\n";
    printList(inst);
    Environment::getLabeledInstruction(lab1)->argument.reg = Register::c;
    Environment::getLabeledInstruction(lab2)->argument.reg = Register::c;
    printList(inst);
    return 0;
}