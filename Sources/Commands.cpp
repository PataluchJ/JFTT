#include "../Includes/Commands.hpp"

Assign::Assign(Identifier* var, Expression* value)
{
	this->var = var;
	this->exp = value;
}

Assign::~Assign()
{
	delete var;
	delete exp;
}

InstructionList* Assign::generate()
{
	auto address = this->var->addressToRegister(Register::c);
	auto val = this->exp->calculateToRegister(Register::a);

	auto inst = new InstructionList;
	inst->splice(inst->end(), *address);
	inst->splice(inst->end(), *val);
	inst->push_back(new Instruction(OptCode::STORE, Register::c));

	delete address;
	delete val; 
	return inst;
}

If::If(Condition* cond, Commands* commands)
{
	this->cond = cond;
	this->commands = commands;
}

If::~If()
{
	delete cond;
	delete commands;
}

InstructionList* If::generate()
{
	auto inst = new InstructionList;

	auto inBlock = commands->generate();
	auto condBlock = cond->generateCondtion(inBlock->size()+1);

	inst->splice(inst->end(), *condBlock);
	inst->splice(inst->end(), *inBlock);

	delete inBlock;
	delete condBlock;

	return inst;
}

If_Else::If_Else(Condition* cond, Commands* ifBlock, Commands* elseBlock)
{
	this->cond = cond;
	this->ifBlock = ifBlock;
	this->elseBlock = elseBlock;
}

If_Else::~If_Else()
{
	delete cond;
	delete ifBlock;
	delete elseBlock;
}

InstructionList* If_Else::generate()
{
	auto elseBlockInst = elseBlock->generate();
	auto ifBlockInst = ifBlock->generate();
	ifBlockInst->push_back(new Instruction(OptCode::JUMP, elseBlockInst->size()+1));
	auto condBlockInst = cond->generateCondtion(ifBlockInst->size() + 1);

	auto inst = new InstructionList;
	inst->splice(inst->end(), *condBlockInst);
	inst->splice(inst->end(), *ifBlockInst);
	inst->splice(inst->end(), *elseBlockInst);
	
	delete condBlockInst;
	delete ifBlockInst;
	delete elseBlockInst;

	return inst;
}

While::While(Condition* cond, Commands* commands)
{
	this->cond = cond;
	this->block = commands;
}

While::~While()
{
	delete cond;
	delete block;
}

InstructionList* While::generate()
{
	auto inst = new InstructionList;

	auto blockInst = block->generate();
	auto condInst = cond->generateCondtion(blockInst->size() + 1);

	inst->splice(inst->end(), *condInst);
	inst->splice(inst->end(), *blockInst);

	delete blockInst;
	delete condInst;

	return inst;
}
