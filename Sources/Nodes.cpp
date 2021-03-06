#include "../Includes/Nodes.hpp"

InstructionList* Condition::subtractToRegister(Value* left, Value* right)
{
	InstructionList* inst = new InstructionList;
	auto genLeftValue = left->valueToRegister(Register::a);
	auto genRightValue = right->valueToRegister(Register::d);
	inst->splice(inst->end(), *genRightValue);
	inst->splice(inst->end(), *genLeftValue);
	inst->push_back(new Instruction(OptCode::SUB, Register::d));

	delete genLeftValue;
	delete genRightValue;

	return inst;
}

Commands::Commands(Command* com){
	this->commands.push_back(com);
}
Commands::~Commands(){
	for(auto& c : commands){
		delete c;
	}
}

InstructionList* Commands::generate(){
	auto inst = new InstructionList;

	for(auto& c : commands){
		inst->splice(inst->end(), *(c->generate()));
	}

	return inst;
}
void Commands::add(Command* com){
	this->commands.push_back(com);
}
