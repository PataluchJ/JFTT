#include "../Includes/Utility.hpp"

void Util::printMemVar(MemVar* var) {
    std::cout << "Adr: " << var->address << " Init: " << var->initialized << " Err: " << var->error << "\n";
}

void Util::printList(InstructionList* list) {
    for (auto l : *list) {
        std::cout << l->toString() << "\n";
    }
}
