#ifndef COMMANDS_HPP 
#define COMMANDS_HPP

#include "Nodes.hpp"
#include "Conditions.hpp"
#include "Expressions.hpp"
#include "Identifiers.hpp"

class Assign : public Command {
public:
	Identifier* var;
	Expression* exp;
	Assign(Identifier* var, Expression* value);
	~Assign();
	InstructionList* generate() override;
};

class If : public Command {
public:
	Condition* cond;
	Commands* commands;

	If(Condition* cond, Commands* commands);
	~If();
	InstructionList* generate() override;
};

class If_Else : public Command {
public:
	Condition* cond;
	Commands* ifBlock;
	Commands* elseBlock;

	If_Else(Condition* cond, Commands* ifBlock, Commands* elseBlock);
	~If_Else();
	InstructionList* generate() override;
}; 

class While : public Command {
public:
	Condition* cond;
	Commands* block;

	While(Condition* cond, Commands* commands);
	~While();
	InstructionList* generate() override;
};

class For : public Command {
public:
	Identifier* iterator;
	Value* iStart;
	Value* iEnd;
	Commands* block;

	Value* iterValue;
	Identifier* rightBoundIden;
	Value* rightBoundVal;
	Assign* setIteratorStartValue;
	Assign* setIteratorRightBound;
	Value* rightBound;
	Assign* increment;
	Condition* cond;
	While* loop;

	bool reverse;
	For(Identifier* iter, Value* iStart, Value* iEnd, Commands* block, bool reverse = false);
	~For();
	InstructionList* generate() override;
};

class Repeat : public Command {
public:
	Condition* cond;
	Commands* block;

	Repeat(Condition* cond, Commands* commands);
	~Repeat();
	InstructionList* generate() override;
};

class Read : public Command {
public:
	Identifier* id;
	Read(Identifier* id);
	~Read();
	InstructionList* generate() override;
};

class Write : public Command {
public:
	Value* val;
	Write(Value* val);
	~Write();
	InstructionList* generate() override;
};

#endif