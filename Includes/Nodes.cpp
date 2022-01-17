#include "../Includes/Nodes.hpp"

InstructionList* Condition::subtractToRegister(Value* left, Value* right)
{
	InstructionList* inst = new InstructionList;
	auto genLeftValue = left->valueToRegister(Register::a);
	auto genRightValue = left->valueToRegister(Register::c);
	inst->splice(inst->end(), *genRightValue);
	inst->splice(inst->end(), *genLeftValue);
	inst->push_back(new Instruction(OptCode::SUB, Register::c));

	delete genLeftValue;
	delete genRightValue;

	return inst;
}
