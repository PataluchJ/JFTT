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
InstructionList* DivExpression::calculateToRegister(Register target){
    InstructionList* inst = new InstructionList;
    /*
        if (b != 0){
            sign = 1
            pOne = 1
            mOne = -1
            r = 0</inir>
            if !(-a<0) { a = -a; sing = -1;}
            if !(-b<0) { b = -b; sign = 0-sign;} d=b
            while !(a-b<0) {b = b<<1;}b = b>>1
            while !(b - d < 0) {
                r = r << 1;  
                if !(a-b<0) { r += 1; a = a - b; } 
                b = b >> 1; 
            }
        }
        else{
            return 0;
        }
    */
    auto a = Register::b;
    auto b = Register::c;
    auto sign = Register::d;
    auto pOne = Register::e;
    auto mOne = Register::f;
    auto d = Register::g;
    auto r = Register::h;

    /*  Load a and b */
    inst->splice(inst->begin(), *(right->valueToRegister(b)));
    inst->splice(inst->begin(), *(left->valueToRegister(a)));
    /* if (b==0) {return 0;} */ 
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JZERO, 2));
    inst->push_back(new Instruction(OptCode::JUMP, 3));
    inst->push_back(new Instruction(OptCode::RESET, r));
    inst->push_back(new Instruction(OptCode::JUMP, 58));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /*  sign = 1; pOne = 1;  mOne = -1; r = 0;*/
    inst->push_back(new Instruction(OptCode::RESET, sign)); // Block size = 7 | 51
    inst->push_back(new Instruction(OptCode::INC, sign));
    inst->push_back(new Instruction(OptCode::RESET, pOne));
    inst->push_back(new Instruction(OptCode::INC, pOne));
    inst->push_back(new Instruction(OptCode::RESET, mOne));
    inst->push_back(new Instruction(OptCode::DEC, mOne));
    inst->push_back(new Instruction(OptCode::RESET, r));
    /* if !(-a < 0) { a = -a; sign = -1;} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 6
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = -a 
    inst->push_back(new Instruction(OptCode::JNEG, 4)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, a)); // swap acc=-a with a 
    inst->push_back(new Instruction(OptCode::DEC, sign)); //
    inst->push_back(new Instruction(OptCode::DEC, sign)); //| sign = -1
    /* if !(-b < 0) { b = -b; sign = 0 - sign} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 7 || 38
    inst->push_back(new Instruction(OptCode::SUB, b)); // acc = -b 
    inst->push_back(new Instruction(OptCode::JNEG, 5)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, b)); // swap acc=-b with b
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); 
    inst->push_back(new Instruction(OptCode::SUB, sign));   //| 
    inst->push_back(new Instruction(OptCode::SWAP, sign));  //| sign = 0-sign
    /* d = b */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 3
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SWAP, d));
    /* while !(a-b<0)  */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::JNEG, 5));
    /* { b = b << 1;} */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 4
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JUMP, -7));
    /* b = b>>1 */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 3
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /* while !(b - d < 0)*/
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SUB, d));
    inst->push_back(new Instruction(OptCode::JNEG, 14));
    /* { *
    /* r = r << 1; */ 
    inst->push_back(new Instruction(OptCode::SWAP, r)); // Block size = 3
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    /* if !(a-b<0)*/
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::JNEG, 3));
    /* { r += 1; a = a - b; } */
    inst->push_back(new Instruction(OptCode::SWAP, a)); // a = acc (acc = a-b) // Block size = 2
    inst->push_back(new Instruction(OptCode::INC, r));
    /* b = b >> 1;  */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 4
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JUMP, -16));
    /* } while */
    /* Set sigh */
    inst->push_back(new Instruction(OptCode::SWAP, sign));
    inst->push_back(new Instruction(OptCode::JPOS, 4));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, r));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    
    /* Put result in desired register */
    if(target != r){
        inst->push_back(new Instruction(OptCode::SWAP, r));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    return inst;
}
InstructionList* ModExpression::calculateToRegister(Register target){
    InstructionList* inst = new InstructionList;
    // Registers
    auto a = Register::b;
    auto b = Register::c;
    auto sign = Register::d;
    auto pOne = Register::e;
    auto mOne = Register::f;
    auto d = Register::g;
    auto r = Register::h;

    /*  Load a and b */
    inst->splice(inst->begin(), *(right->valueToRegister(b)));
    inst->splice(inst->begin(), *(left->valueToRegister(a)));
    /* if (b==0) {return 0;} */ 
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JZERO, 2));
    inst->push_back(new Instruction(OptCode::JUMP, 3));
    inst->push_back(new Instruction(OptCode::RESET, a));
    inst->push_back(new Instruction(OptCode::JUMP, 56));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /*  sign = 1; pOne = 1;  mOne = -1; r = 0;*/
    inst->push_back(new Instruction(OptCode::RESET, sign)); // Block size = 7 | 51
    inst->push_back(new Instruction(OptCode::INC, sign));
    inst->push_back(new Instruction(OptCode::RESET, pOne));
    inst->push_back(new Instruction(OptCode::INC, pOne));
    inst->push_back(new Instruction(OptCode::RESET, mOne));
    inst->push_back(new Instruction(OptCode::DEC, mOne));
    inst->push_back(new Instruction(OptCode::RESET, r));
    /* if !(-a < 0) { a = -a; sign = -1;} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 6
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = -a 
    inst->push_back(new Instruction(OptCode::JNEG, 2)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, a)); // swap acc=-a with a 
    /* if !(-b < 0) { b = -b; sign = 0 - sign} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 7 || 38
    inst->push_back(new Instruction(OptCode::SUB, b)); // acc = -b 
    inst->push_back(new Instruction(OptCode::JNEG, 5)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, b)); // swap acc=-b with b
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); 
    inst->push_back(new Instruction(OptCode::DEC, sign)); //
    inst->push_back(new Instruction(OptCode::DEC, sign)); //| sign = -1
    /* d = b */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 3
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SWAP, d));
    /* while !(a-b<0)  */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::JNEG, 5));
    /* { b = b << 1;} */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 4
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JUMP, -7));
    /* b = b>>1 */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 3
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /* while !(b - d < 0)*/
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SUB, d));
    inst->push_back(new Instruction(OptCode::JNEG, 14));
    /* { *
    /* r = r << 1; */
    inst->push_back(new Instruction(OptCode::SWAP, r)); // Block size = 3
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    /* if !(a-b<0)*/
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 4
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::JNEG, 3));
    /* { r += 1; a = a - b; } */
    inst->push_back(new Instruction(OptCode::SWAP, a)); // a = acc (acc = a-b) // Block size = 2
    inst->push_back(new Instruction(OptCode::INC, r));
    /* b = b >> 1;  */
    inst->push_back(new Instruction(OptCode::SWAP, b)); // Block size = 4
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JUMP, -16));
    /* } while */
    /* Set sigh */
    inst->push_back(new Instruction(OptCode::SWAP, sign));
    inst->push_back(new Instruction(OptCode::JPOS, 4));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, a));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    
    /* Put result in desired register */
    if(target != a){
        inst->push_back(new Instruction(OptCode::SWAP, a));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    return inst;
}