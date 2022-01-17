#include "../Includes/Environment.hpp"

std::map<std::string, MemVar*> Environment::variables;
std::vector<Instruction*> Environment::labels;
size_t Environment::nextFree = 0;
size_t Environment::nextLabel = 0;
MemVar* Environment::undef = new MemVar(false,true,0);

size_t Environment::reserve(size_t size) {
    size_t address = Environment::nextFree;
    Environment::nextFree += size;
    return address;
}
void Environment::declareVariable(std::string name){
    Logger::log("Declaring variable " + name);
    if(variables.find(name) != variables.end()){
        std::string errMessage = "Variable " + name + " redefinition.";
        Logger::err(errMessage);
        variables[name]->error = true;
        return;
    }
    MemVar* var = new MemVar;
    var->address = reserve(1);
    var->initialized = false;
    var->error = false;
    var->leftBound = 0;
    var->rightBound = 0;
    variables[name] = var;
}
void Environment::declareArray(std::string name, NumberType leftBound, NumberType rightBound){
    Logger::log("Declaring array " + name);
    if(variables.find(name) != variables.end()){
        variables[name]->error = true;
        std::string errMessage = "Variable " + name + " redefinition.";
        Logger::err(errMessage);
        return;
    }
    MemVar* var = new MemVar;
    var->error = false;
    var->initialized = false;
    if(leftBound > rightBound){
        var->address = 0;
        var->leftBound = 0;
        var->rightBound = 0;
        var->error = true;
        std::string errMessage = "Array " + name + " definition has wrong indexes.";
        Logger::err(errMessage);
    }
    else{
        var->address = reserve(rightBound-leftBound);
        var->leftBound = leftBound;
        var->rightBound = rightBound;
        }
    variables[name] = var;
    

}
MemVar* Environment::getVariable(std::string name){
    Logger::log("Accessing variable " + name);
    if(variables.find(name) == variables.end()){
        std::string errMessage = "Variable " + name + " is not defined.";
        Logger::err(errMessage);
        return undef;
    }
    return variables[name];
}

size_t Environment::laberInstruction(Instruction* inst){
    Logger::log("Labeling instruction with label " + std::to_string(nextLabel));
    size_t label = nextLabel;
    labels.push_back(inst);
    nextLabel++;
    return label;
}
Instruction* Environment::getLabeledInstruction(size_t label){
    Logger::log("Accessing labeled instruction with label " + std::to_string(label));
    return labels[label];
}

MemVar::MemVar() {} 
MemVar::MemVar(bool init, bool err, size_t address) 
    : initialized(init), error(err), address(address) {}