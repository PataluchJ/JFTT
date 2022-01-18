#include "../Includes/Expressions.hpp"

ConstExpression::ConstExpression(Value* value){
    this->val = value;
}
AddExpression::AddExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
SubExpression::SubExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
MulExpression::MulExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
DivExpression::DivExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}
ModExpression::ModExpression(Value* left, Value* right){
    this->left = left;
    this->right = right;
}

ConstExpression::~ConstExpression(){
    delete val;
}
AddExpression::~AddExpression(){
    delete left;
    delete right;
}
SubExpression::~SubExpression(){
    delete left;
    delete right;
}
MulExpression::~MulExpression(){
    delete left;
    delete right;
}
DivExpression::~DivExpression(){
    delete left;
    delete right;
}
ModExpression::~ModExpression(){
    delete left;
    delete right;
}

InstructionList* ConstExpression::calculateToRegister(Register r){
    auto inst = this->val->valueToRegister(r);
    return inst;
}
InstructionList* AddExpression::calculateToRegister(Register r){
    if(this->left->isConst() && this->right->isConst()){
        NumberType value = ((ConstValue*)left)->value + ((ConstValue*)right)->value;
        return generateNumber(value, r);
    }
    else if(this->left->isConst() || this->right->isConst()){
        Value* constant = left->isConst() ? left : right;
        Value* variable = left->isConst() ? right : left;
        if(((ConstValue*)constant)->value == 1){
            auto inst = variable->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::INC, r));
            return inst;
        }
        else if (((ConstValue*)constant)->value == -1){
            auto inst = variable->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::DEC, r));
            return inst;
        }
    }
    auto inst = this->right->valueToRegister(Register::c);
    auto loadLeftToA = this->left->valueToRegister(Register::a);
    //inst->merge(*loadLeftToA);
    inst->splice(inst->end(), *loadLeftToA);
    inst->push_back(new Instruction(OptCode::ADD, Register::c));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete loadLeftToA;
    return inst;
}
InstructionList* SubExpression::calculateToRegister(Register r){
    if(this->left->isConst() && this->right->isConst()){
        NumberType value = ((ConstValue*)left)->value - ((ConstValue*)right)->value;
        return generateNumber(value, r);
    }
    else if(this->right->isConst()){
        if(((ConstValue*)right)->value == 1){
            auto inst = this->left->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::DEC, r));
            return inst;
        }
        else if(((ConstValue*)right)->value == -1){
            auto inst = this->left->valueToRegister(r);
            inst->push_back(new Instruction(OptCode::INC, r));
            return inst;
        }
    }
    auto inst = this->right->valueToRegister(Register::c);
    auto loadLeftToA = this->left->valueToRegister(Register::a);
    //inst->merge(*loadLeftToA);
    inst->splice(inst->end(), *loadLeftToA);
    inst->push_back(new Instruction(OptCode::SUB, Register::c));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete loadLeftToA;
    return inst;
}
InstructionList* MulExpression::calculateToRegister(Register r){
    /*
    #####  |a| > |b| oraz b > 0 
    c = a
    d = b
    if a < 0 
     c = -a
    else 
     c = a
    if b < 0
     d = -b
    else 
     d = b 
    if b - c > 0 
     swap a <> b  
    #####   
    08 if b == 0 JUMP 14
    09 if b - (b>>1)<<1 == 0 JUMP 11
    10 r += a
    11 a = a<<1
    12 b = b>>1
    13 JUMP 8
    14 return r
    Registers:
    rb = a, rc=b rd=|a| 
    */
    InstructionList* inst = new InstructionList;
    // rc = b 
    auto loadBToC = this->right->valueToRegister(Register::c);
    // rd = a 
    auto loadAToB = this->left->valueToRegister(Register::b);
    
    inst->splice(inst->end(), *loadBToC);
    inst->splice(inst->end(), *loadAToB); 

    delete loadBToC;
    delete loadAToB;
    // Start: rb = a , rc = b 
    // End |rb| > |rc|, Using rd = |a| re=|b|
    // rd = |a|
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, Register::b));
    inst->push_back(new Instruction(OptCode::JPOS, 3));
    inst->push_back(new Instruction(OptCode::RESET, Register::a ));
    inst->push_back(new Instruction(OptCode::ADD, Register::b ));
    inst->push_back(new Instruction(OptCode::SWAP, Register::d ));
    // ra = |b|
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, Register::c));
    inst->push_back(new Instruction(OptCode::JPOS, 3));
    inst->push_back(new Instruction(OptCode::RESET, Register::a ));
    inst->push_back(new Instruction(OptCode::ADD, Register::c ));
    // ra = |b| - |a| 
    inst->push_back(new Instruction(OptCode::SUB, Register::d));
    // if ra > 0 swap rb <> rc
    inst->push_back(new Instruction(OptCode::JNEG, 4));
    inst->push_back(new Instruction(OptCode::SWAP, Register::b));
    inst->push_back(new Instruction(OptCode::SWAP, Register::c));
    inst->push_back(new Instruction(OptCode::SWAP, Register::b));
    // a * b where |a| > |b| 
    // Start: rb = a rc = b
    // Using: re = helper for last bit of b, rd = -1, re = 1, rf = result
    // Prepare rd, re, rf
    inst->push_back(new Instruction(OptCode::RESET, Register::d));
    inst->push_back(new Instruction(OptCode::DEC, Register::d));
    inst->push_back(new Instruction(OptCode::RESET, Register::e));
    inst->push_back(new Instruction(OptCode::INC, Register::e));
    inst->push_back(new Instruction(OptCode::RESET, Register::f));
    /// while b != 0 
    inst->push_back(new Instruction(OptCode::SWAP, Register::c));
    inst->push_back(new Instruction(OptCode::JZERO, 18));
    
    inst->push_back(new Instruction(OptCode::SWAP, Register::c));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::ADD, Register::c)); // ra = b 
    inst->push_back(new Instruction(OptCode::SHIFT, Register::d));
    inst->push_back(new Instruction(OptCode::SHIFT, Register::e)); // ra = b & 0b111..10
    inst->push_back(new Instruction(OptCode::SUB, Register::c)); // ra = ((b>>1)<<1) - b

    inst->push_back(new Instruction(OptCode::JZERO, 4)); // Last bit was zero, skip adding to f
    inst->push_back(new Instruction(OptCode::SWAP, Register::f));
    inst->push_back(new Instruction(OptCode::ADD, Register::b)); // res += a
    inst->push_back(new Instruction(OptCode::SWAP, Register::f));

    inst->push_back(new Instruction(OptCode::SWAP, Register::b)); // ra = a
    inst->push_back(new Instruction(OptCode::SHIFT, Register::e)); // ra = a*2
    inst->push_back(new Instruction(OptCode::SWAP, Register::b));

    inst->push_back(new Instruction(OptCode::SWAP, Register::c)); // ra = b
    inst->push_back(new Instruction(OptCode::SHIFT, Register::d)); // ra = b/2
    inst->push_back(new Instruction(OptCode::SWAP, Register::c));
    inst->push_back(new Instruction(OptCode::JUMP, -18));

    inst->push_back(new Instruction(OptCode::SWAP, Register::f));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    return inst;
}
InstructionList* DivExpression::calculateToRegister(Register r){
    InstructionList* inst = new InstructionList;
    return inst;
}
InstructionList* ModExpression::calculateToRegister(Register r){
    InstructionList* inst = new InstructionList;
    return inst;
}