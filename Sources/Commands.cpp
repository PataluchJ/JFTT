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
	Logger::log("Assign: {");
	Logger::indent += 1;
	auto address = this->var->addressToRegister(Register::g);
	auto val = this->exp->calculateToRegister(Register::h);

	auto inst = new InstructionList;
	inst->splice(inst->end(), *val);
	inst->splice(inst->end(), *address);
	inst->push_back(new Instruction(OptCode::SWAP, Register::h));
	inst->push_back(new Instruction(OptCode::STORE, Register::g));
	Logger::indent -= 1;
	Logger::log("}Assign");
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
	Logger::log("If:(inBlock, condBlock) {");
	Logger::indent += 1;

	auto inBlock = commands->generate();
	auto condBlock = cond->generateCondtion(inBlock->size()+1);

	inst->splice(inst->end(), *condBlock);
	inst->splice(inst->end(), *inBlock);

	Logger::indent -= 1;
	Logger::log("}If");
	

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
	auto condBlockInst = cond->generateCondtion(ifBlockInst->size()+1);

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
	Logger::log("While{");
	Logger::indent += 1;
	auto blockInst = block->generate();
	auto condInst = cond->generateCondtion(blockInst->size()+2);

	inst->splice(inst->end(), *condInst);
	inst->splice(inst->end(), *blockInst);
	inst->push_back(new Instruction(OptCode::JUMP, (-1) *( inst->size() + 1)));

	Logger::indent -= 1;
	Logger::log("}While");
	delete blockInst;
	delete condInst;

	return inst;
}

For::For(Identifier* iter, Value* iStart, Value* iEnd, Commands* block, bool reverse){
	this->iterator = iter;
	this->iterValue = new VarValue(iter);
	this->iStart = iStart;
	this->iEnd = iEnd;
	this->block = block;
	this->reverse = reverse;
	this->setIteratorStartValue = new Assign(iter, new ConstExpression(iStart));

	std::string* rightBoundVarName = new std::string(*(iter->name)+"@end");
	this->rightBoundIden = new Variable(rightBoundVarName);
	this->rightBoundVal = new VarValue(this->rightBoundIden);
	this->setIteratorRightBound = new Assign(this->rightBoundIden, new ConstExpression(this->iEnd));
	if(reverse) {
		this->increment = new Assign(iter, new AddExpression(iterValue, new ConstValue(-1)));
		this->cond = new NotLesser(iterValue, this->rightBoundVal);
	}
	else {
		this->increment = new Assign(iter, new AddExpression(iterValue, new ConstValue(1)));
		this->cond = new NotGreater(iterValue, this->rightBoundVal);
	}
	this->block->add(this->increment);
	this->loop = new While(cond, block);
}

For::~For(){
	delete this->iterator;
	delete this->block;
}

InstructionList* For::generate(){
	auto inst = this->setIteratorStartValue->generate();
	inst->splice(inst->end(), *(this->setIteratorRightBound->generate()));
	inst->splice(inst->end(), *(this->loop->generate()));
	return inst;
}

Repeat::Repeat(Condition* cond, Commands* commands)
{
	this->cond = cond;
	this->block = commands;
}

Repeat::~Repeat()
{
	delete cond;
	delete block;
}

InstructionList* Repeat::generate()
{
	auto inst = new InstructionList;
	Logger::log("Repeat{");
	Logger::indent += 1;
	auto blockInst = block->generate();
	auto condInst = cond->generateCondtion(2);
	
	inst->splice(inst->end(), *blockInst);
	inst->splice(inst->end(), *condInst);
	inst->push_back(new Instruction(OptCode::JUMP, (-1)*(inst->size())));

	Logger::indent -= 1;
	Logger::log("}Repeat");
	delete blockInst;
	delete condInst;

	return inst;
}

Read::Read(Identifier* id) {
	this->id = id;
}
Read::~Read(){
	delete id;
}	
InstructionList* Read::generate(){
	auto inst = new InstructionList;

	auto addressToReg = id->addressToRegister(Register::b);

	inst->splice(inst->end(), *addressToReg);
	inst->push_back(new Instruction(OptCode::GET));
	inst->push_back(new Instruction(OptCode::STORE, Register::b));

	delete addressToReg;

	return inst;
}

Write::Write(Value* val){
	this->val = val;
}
Write::~Write(){
	delete val;
}
InstructionList* Write::generate(){
	auto inst = new InstructionList;
	
	auto valToReg = val->valueToRegister(Register::c);
	
	inst->splice(inst->end(), *valToReg);
	inst->push_back(new Instruction(OptCode::SWAP, Register::c));
	inst->push_back(new Instruction(OptCode::PUT));

	delete valToReg;

	return inst;
}