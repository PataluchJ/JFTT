#include "../Identifiers.hpp"

void printList(InstructionList* list){
    for(auto l : *list){
        std::cout << l->toString() << "\n";
    }
}

int main(){
    Environment::declareVariable("n");
    Environment::declareArray("array", -5,5);
    std::string n_name = "n";
    std::string array_name = "array";
    Identifier* var = new Variable(&n_name);
    Identifier* arrConst = new ArrayByConst(&array_name, 2);
    Identifier* arrVar = new ArrayByVariable(&array_name, &n_name);

    auto varInst = var->addressToRegister(Register::a);
    auto constInst = arrConst->addressToRegister(Register::a);
    auto varArInst = arrVar->addressToRegister(Register::c);

    std::cout << "Variable to register a:\n";
    printList(varInst);
    std::cout << "Array by const to register a:\n";
    printList(constInst);
    std::cout << "Array by variable to register c:\n";
    printList(varArInst);

    deleteInstructionList(varInst);
    deleteInstructionList(constInst);
    deleteInstructionList(varArInst);
}