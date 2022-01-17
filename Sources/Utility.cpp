#include "../Includes/Utility.hpp"

void Util::printMemVar(MemVar* var) {
    std::cout << "Adr: " << var->address << " Init: " << var->initialized << " Err: " << var->error << "\n";
}

void Util::printList(InstructionList* list) {
    for (auto l : *list) {
        std::cout << l->toString() << "\n";
    }
}

void Util::saveASM(InstructionList* list, std::string filename){
    std::ofstream file(filename);
    if(!file.good()){
        std::cout << "Failed to open outfile.\n";
        return;
    }
    for(auto& i : *list){
        file << i->toString() << "\n";
    }
    file << "HALT\n";

    file.close();
}