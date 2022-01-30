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
    auto inst = this->right->valueToRegister(Register::d);
    auto loadLeftToA = this->left->valueToRegister(Register::b);
    
    inst->splice(inst->end(), *loadLeftToA);
    inst->push_back(new Instruction(OptCode::SWAP, Register::b));
    inst->push_back(new Instruction(OptCode::ADD, Register::d));
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
    auto inst = this->right->valueToRegister(Register::d);
    auto loadLeftToA = this->left->valueToRegister(Register::b);
    //inst->merge(*loadLeftToA);
    inst->splice(inst->end(), *loadLeftToA);
    inst->push_back(new Instruction(OptCode::SWAP, Register::b));
    inst->push_back(new Instruction(OptCode::SUB, Register::d));
    if(r != Register::a)
        inst->push_back(new Instruction(OptCode::SWAP, r));

    delete loadLeftToA;
    return inst;
}
InstructionList* MulExpression::calculateToRegister(Register target){
    /*
    Dla a*b
    # |a| > |b| and b>0
    c = a;
    if c < 0 {c = 0-c;}
    d = b;
    if d < 0 {d = 0-d;}
    if d-c > 0 {a swap b}
    # discard c,d
    # multiplication 
    use r, h(accu), mOne, pOne
    mOne = -1;
    pOne = 1;
    r = 0;
    while b > 0 {
        h = ((b shift mOne) shit pOne);
        if !(b-h == 0){
            r = r + a;
        }
        b = b shift mOne;
        a = a shift mPlus;
    }
    r swap target
    */

    auto a = Register::e;
    auto b = Register::f;
    auto c = Register::b;
    auto d = Register::c;
    auto r = Register::g;
    auto h = Register::b;
    auto mOne = Register::c;
    auto pOne = Register::d;
    auto accu = Register::a;
    InstructionList* inst = new InstructionList;

    auto loadB = this->right->valueToRegister(b);
    auto loadA = this->left->valueToRegister(a);
    
    inst->splice(inst->end(), *loadB);
    inst->splice(inst->end(), *loadA); 

    /* c=a; d=b; */
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SWAP, c));
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SWAP, d));
    /* if !(c > 0) { c = 0-c;} => c = |a| */
    inst->push_back(new Instruction(OptCode::SWAP, c));
    inst->push_back(new Instruction(OptCode::JPOS, 3));
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::SUB, a));
    inst->push_back(new Instruction(OptCode::SWAP, c));
    /* if !(d>0) { d = 0-d;} => d = |b| */
    inst->push_back(new Instruction(OptCode::SWAP, d));
    inst->push_back(new Instruction(OptCode::JPOS, 3));
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::SUB, b));
    // inst->push_back(new Instruction(OptCode::SWAP, d)); Leave d in accu 
    /* if !(d - c < 0) { a swap b} => |a| > |b|*/
    inst->push_back(new Instruction(OptCode::SUB, a));
    inst->push_back(new Instruction(OptCode::JNEG, 4));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    /* r = 0; mOne = -1; pOne = 1; */
    inst->push_back(new Instruction(OptCode::RESET, r));
    inst->push_back(new Instruction(OptCode::RESET, mOne));
    inst->push_back(new Instruction(OptCode::DEC, mOne));
    inst->push_back(new Instruction(OptCode::RESET, pOne));
    inst->push_back(new Instruction(OptCode::INC, pOne));
    /* while ! b == 0  */
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JZERO, 18));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /* acc is h;  h = 0; h = b; h = h shift mOne; h = h shift pOne; */
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::ADD, b));
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    /* if !( b != h) */
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::JZERO, 4));
    /* { r = r+a; } */
    inst->push_back(new Instruction(OptCode::SWAP, r));
    inst->push_back(new Instruction(OptCode::ADD, a));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    /* b = b shift mOne (/2) */
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::SHIFT, mOne));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    /* a = a shift mPlus (*2) */ 
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::SHIFT, pOne));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::JUMP, -18));
    /* end while */
    /* Swap r with target register */
    if (r != target){
        inst->push_back(new Instruction(OptCode::SWAP, r));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    delete loadB;
    delete loadA;
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
    auto a = Register::d;
    auto b = Register::e;
    auto sign = Register::b;
    auto pOne = Register::c;
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
    inst->push_back(new Instruction(OptCode::JUMP, 59));
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
    inst->push_back(new Instruction(OptCode::JPOS, 5));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, r));
    inst->push_back(new Instruction(OptCode::DEC, Register::a));
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