#include "../Includes/Conditions.hpp"

InstructionList* Equal::generateCondtion(size_t jumpOutOffset)
{
	/*
	rc <- right
	ra <- left
	SUB rc // ra = left - rigth
	JZERO 2
	JUMP jumoOutOffset
	*/
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JZERO, 2));
	inst->push_back(new Instruction(OptCode::JUMP, jumpOutOffset));

	return inst;
}
InstructionList* NotEqual::generateCondtion(size_t jumpOutOffset)
{
	/*
	rc <- right
	ra <- left 
	SUB rc // ra = left-right 
	JZERO jumpOutOffset 
	*/
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JZERO, jumpOutOffset));

	return inst;
}
InstructionList* Lesser::generateCondtion(size_t jumpOutOffset)
{
	/*
	rc <- right
	ra <- left
	SUB rc // ra = left-right
	JNEG 2
	JUMP jumpOutOffset 
	*/
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JNEG, 2));
	inst->push_back(new Instruction(OptCode::JUMP, jumpOutOffset));

	return inst;
}
InstructionList* Greater::generateCondtion(size_t jumpOutOffset)
{
	/*
	rc <- right
	ra <- left
	SUB rc // ra = left-right
	JPOS 2
	JUMP out
	*/
	Logger::log("Greater{");
	Logger::indent += 1;
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JPOS, 2));
	inst->push_back(new Instruction(OptCode::JUMP, jumpOutOffset));
	Logger::indent -= 1;
	Logger::log("}Greater");
	return inst;
}
InstructionList* NotLesser::generateCondtion(size_t jumpOutOffset)
{
	/* left >= right
	rc <- right
	ra <- left
	SUB rc // ra = left-right ! < 0
	JNEG out
	*/
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JNEG, jumpOutOffset));

	return inst;
}
InstructionList* NotGreater::generateCondtion(size_t jumpOutOffset)
{
	/*
	rc <- right
	ra <- left
	SUB rc // ra = left-right
	JPOS out
	*/
	auto inst = Condition::subtractToRegister(left, right);
	inst->push_back(new Instruction(OptCode::JPOS, jumpOutOffset));

	return inst;
}


Equal::Equal(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

NotEqual::NotEqual(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

Lesser::Lesser(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

Greater::Greater(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

NotLesser::NotLesser(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

NotGreater::NotGreater(Value* left, Value* right)
{
	this->left = left;
	this->right = right;
}

Equal::~Equal()
{
	delete left;
	delete right;
}

NotEqual::~NotEqual()
{
	delete left;
	delete right;
}

Lesser::~Lesser()
{
	delete left;
	delete right;
}

Greater::~Greater()
{
	delete left;
	delete right;
}

NotLesser::~NotLesser()
{
	delete left;
	delete right;
}


NotGreater::~NotGreater()
{
	delete left;
	delete right;
}

