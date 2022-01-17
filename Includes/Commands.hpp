#ifndef COMMANDS_HPP 
#define COMMANDS_HPP

#include "Nodes.hpp"

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
#endif