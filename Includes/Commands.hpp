#ifndef COMMANDS_HPP 
#define COMMANDS_HPP

#include "Nodes.hpp"

class Assign : public Node {
public:
	Identifier* var;
	Expression* exp;
	Assign(Identifier* var, Expression* value);
	InstructionList* generate();
};

class While : public Node {
public:
	
};

#endif