#ifndef CONDITIONS_HPP
#define CONDITIONS_HPP

#include "Nodes.hpp"

class Equal : public Condition {
public:
	Equal(Value* left, Value* right);
	~Equal();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};

class NotEqual : public Condition {
public:
	NotEqual(Value* left, Value* right);
	~NotEqual();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};

class Lesser : public Condition {
public:
	Lesser(Value* left, Value* right);
	~Lesser();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};
class Greater : public Condition {
public:
	Greater(Value* left, Value* right);
	~Greater();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};
class NotLesser : public Condition {
public:
	NotLesser(Value* left, Value* right);
	~NotLesser();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};
class NotGreater : public Condition {
public:
	NotGreater(Value* left, Value* right);
	~NotGreater();
	InstructionList* generateCondtion(size_t jumpOutOffset) override;
};
#endif