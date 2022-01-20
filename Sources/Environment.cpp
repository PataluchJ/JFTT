#include "../Includes/Environment.hpp"

std::map<std::string, MemVar*> Environment::variables;
std::vector<Instruction*> Environment::labels;
size_t Environment::nextFree = 0;
size_t Environment::nextLabel = 0;
MemVar* Environment::undef = new MemVar(false,true,0);
std::list<std::string> Environment::variableDeclarations;
std::list<std::string> Environment::iteratorDeclarations;
std::list<Environment::ArrayDeclaration> Environment::arraysDeclarations;
bool Environment::declarationsFailed = false;

Environment::ArrayDeclaration::ArrayDeclaration(std::string name, NumberType left, NumberType right) 
    : name(name), leftBound(left), rightBound(right) {}

MemVar::MemVar(size_t address, MemVarType type){
    this->address = address;
    this->type = type;
    this->leftBound = 0;
    this->rightBound = 0;
    this->error = false;
    this->error = true;
}
MemVar::MemVar(size_t address, NumberType left, NumberType right){
    this->address = address;
    this->type = MemVar::MemVarType::Array;
    this->leftBound = left;
    this->rightBound = right;
    this->error = false;
    this->error = true;
}

size_t Environment::reserve(size_t size) {
    size_t address = Environment::nextFree;
    Environment::nextFree += size;
    return address;
}
void Environment::declareVariable(std::string name){
    variableDeclarations.push_back(name);
}
void Environment::declareIterator(std::string name){
    for(auto& existingName : iteratorDeclarations){
        if(existingName == name)
            return;
    }
    iteratorDeclarations.push_back(name);
}

void Environment::declareArray(std::string name, NumberType leftBound, NumberType rightBound){
    arraysDeclarations.push_back(ArrayDeclaration(name, leftBound, rightBound));
}

void Environment::addToMemory(std::string name, MemVar* var){
    if(variables.find(name) != variables.end()){
        variables[name]->error = true;
        std::string errMessage = "Variable " + name + " redefinition.";
        Logger::err(errMessage);
        Environment::declarationsFailed = true;
        return;
    }
    variables[name] = var;
}

void Environment::finalizeDeclarations(){
    MemVar* var;
    for(auto& name  : variableDeclarations){
        Logger::log("Declared variable: " + name + " addr " + std::to_string(nextFree));
        var = new MemVar(Environment::nextFree, MemVar::MemVarType::Normal);
        Environment::addToMemory(name, var);
        nextFree += 1;
    }
    for(auto& name  : iteratorDeclarations){
        Logger::log("Declared iterator: " + name + " addr " + std::to_string(nextFree));
        var = new MemVar(Environment::nextFree, MemVar::MemVarType::Iterator);
        Environment::addToMemory(name, var);
        nextFree += 1;
        var = new MemVar(Environment::nextFree, MemVar::MemVarType::Iterator);
        Environment::addToMemory(name+"@end", var);

        nextFree += 1;
    }
    for(auto& arr  : arraysDeclarations){
        Logger::log("Declared array: " + arr.name + " addr " + std::to_string(nextFree) + " offseted to " + std::to_string(Environment::nextFree-arr.leftBound) );
        if(arr.leftBound > arr.rightBound){
            Logger::err("Failed to declare \"" + arr.name + "\", left bound > right bound.");
            Environment::declarationsFailed = true;
        }
        var = new MemVar(Environment::nextFree-arr.leftBound, arr.leftBound, arr.rightBound);
        Environment::addToMemory(arr.name, var);
        nextFree += 1+(arr.rightBound-arr.leftBound);
    }

}

MemVar* Environment::getVariable(std::string name){
    //Loggeg("Accessing variable " + name);
    if(variables.find(name) == variables.end()){
        std::string errMessage = "Variable " + name + " is not defined.";
        Logger::err(errMessage);
        return undef;
    }
    return variables[name];
}

size_t Environment::laberInstruction(Instruction* inst){
    //Loggeg("Labeling instruction with label " + std::to_string(nextLabel));
    size_t label = nextLabel;
    labels.push_back(inst);
    nextLabel++;
    return label;
}
Instruction* Environment::getLabeledInstruction(size_t label){
    //Logge::log("Accessing labeled instruction with label " + std::to_string(label));
    return labels[label];
}

MemVar::MemVar() {} 
MemVar::MemVar(bool init, bool err, size_t address) 
    : initialized(init), error(err), address(address) {}