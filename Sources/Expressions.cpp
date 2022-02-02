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

bool ConstExpression::validate(int line){
    if(!this->val->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)val)->id->name) + "\" at line " + std::to_string(line));
        return false;
    }
    return true;
}
bool AddExpression::validate(int line){
    bool res = true;
    if(!left->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    if(!right->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    return res;
}
bool SubExpression::validate(int line){
    bool res = true;
    if(!left->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    if(!right->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    return res;
}
bool MulExpression::validate(int line){
    bool res = true;
    if(!left->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    if(!right->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    return res;
}
bool DivExpression::validate(int line){
    bool res = true;
    if(!left->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    if(!right->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    return res;
}
bool ModExpression::validate(int line){
    bool res = true;
    if(!left->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    if(!right->isInit()){
        Logger::log("Using not initialized variable \"" + *(((VarValue*)left)->id->name) + "\" at line " + std::to_string(line));
        res = false;
    }
    return res;
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
    if(this->left->isConst() && this->right->isConst())
        return generateForBothConst(target);
    if(this->left->isConst() || this->right->isConst())
        return generateForOneConst(target);
    return generateForNoConst(target);
}

InstructionList* MulExpression::generateForBothConst(Register target){
    NumberType value = ((ConstValue*)left)->value * ((ConstValue*)right)->value;
    return generateNumber(value, target);
}
InstructionList* MulExpression::generateForOneConst(Register target){
    auto constant = this->left->isConst() ? left : right;
    auto var = this->left->isConst() ? right : left;
    auto value = ((ConstValue*)constant)->value;
    bool vNeg = false;
    if(value < 0){
        value = -value;
        vNeg = true;
    }
    Register pOne = Register::b;
    Register x = Register::c;

    auto inst = var->valueToRegister(x);
    inst->push_back(new Instruction(OptCode::RESET, pOne));
    inst->push_back(new Instruction(OptCode::INC, pOne));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));

    NumberType mask = 0b1;
    bool sumInAcu = true;
    while(mask <= value){
        if((value&mask) > 0){
            // Add x to sum
            if(!sumInAcu)
                inst->push_back(new Instruction(OptCode::SWAP, x));
            inst->push_back(new Instruction(OptCode::ADD, x));
            sumInAcu = true;
        }
        // x <<= 1 
        mask <<= 1;
        if(sumInAcu)
            inst->push_back(new Instruction(OptCode::SWAP, x));
        inst->push_back(new Instruction(OptCode::SHIFT, pOne));
        sumInAcu = false;
    }
    
    if(target != x){
        if(!sumInAcu)
            inst->push_back(new Instruction(OptCode::SWAP, x));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    if(vNeg){
        inst->push_back(new Instruction(OptCode::RESET, Register::a));
        inst->push_back(new Instruction(OptCode::SUB, target));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    return inst;

}
InstructionList* MulExpression::generateForNoConst(Register target){
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
    auto sign = Register::b;
    auto r = Register::g;
    auto mOne = Register::c;
    auto pOne = Register::d;
    auto accu = Register::a;
    InstructionList* inst = new InstructionList;

    auto loadB = this->right->valueToRegister(b);
    auto loadA = this->left->valueToRegister(a);
    
    inst->splice(inst->end(), *loadB);
    inst->splice(inst->end(), *loadA); 
    /* sign = 1 */
    inst->push_back(new Instruction(OptCode::RESET, sign));
    inst->push_back(new Instruction(OptCode::INC, sign));
    /* if !(a > 0) { a = a-c; sign = -1} */
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::JNEG, 3));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::JUMP, 6));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::SUB, a));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::DEC, sign));
    inst->push_back(new Instruction(OptCode::DEC, sign));
    /* if !(b>0) { b = -b; sign = 0-sign}  */
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JNEG, 3));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::JUMP, 7));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::SUB, b));
    inst->push_back(new Instruction(OptCode::SWAP, b));
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::SUB, sign));
    inst->push_back(new Instruction(OptCode::SWAP, sign));
    // inst->push_back(new Instruction(OptCode::SWAP, d)); Leave d in accu 
    /* if !(b - a < 0) { a swap b} => |a| > |b|*/
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::ADD, b));
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
    /* Set sign*/
    inst->push_back(new Instruction(OptCode::SWAP, sign));
    inst->push_back(new Instruction(OptCode::JNEG, 2));
    inst->push_back(new Instruction(OptCode::JUMP, 4));
    inst->push_back(new Instruction(OptCode::RESET, accu));
    inst->push_back(new Instruction(OptCode::SUB, r));
    inst->push_back(new Instruction(OptCode::SWAP, r));
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
    if(this->left->isConst() && this->right->isConst())
        return generateForBothConst(target);
    //if(this->left->isConst() || this->right->isConst())
    //    return generateForOneConst(target);
    return generateForNoConst(target);
}

InstructionList* DivExpression::generateForBothConst(Register target){
    NumberType l = ((ConstValue*)left)->value;
    NumberType r = ((ConstValue*)right)->value;
    NumberType d = l / r;
    if (d < 0){
        if(d*r != l)
            d -= 1;
    }
    return generateNumber(d, target);
}
InstructionList* DivExpression::generateForNoConst(Register target){
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
    inst->push_back(new Instruction(OptCode::JPOS, 7));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, r));
    inst->push_back(new Instruction(OptCode::SWAP, r));
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::JZERO, 2));
    inst->push_back(new Instruction(OptCode::DEC, r));
    
    /* Put result in desired register */
    if(target != r){
        inst->push_back(new Instruction(OptCode::SWAP, r));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }
    return inst;
}

InstructionList* ModExpression::calculateToRegister(Register target){
    if(this->left->isConst() && this->right->isConst())
        return generateForBothConst(target);
    if(this->right->isConst()){
        auto v = ((ConstValue*)(this->right))->value;
        v = v < 0 ? -v : v;
        NumberType mask = 0b1;
        while(mask <= v){
            mask <<= 1;
        }
        mask >>= 1;
        if(mask == v){
            return generateForTwoPow(target);
        }
    }
    return generateForNoConst(target);
}

InstructionList* ModExpression::generateForBothConst(Register target) {
    NumberType l = ((ConstValue*)left)->value;
    NumberType r = ((ConstValue*)right)->value;
    bool lNeg = l < 0;
    l = l < 0 ? -l : l;
    bool rNeg = r < 0;
    r = r < 0 ? -r : r;
    NumberType value = l % r;
    if(lNeg)
        value = r - value;
    if(rNeg)
        value = 0-value;
    return generateNumber(value, target);
}
InstructionList* ModExpression::generateForTwoPow(Register target){
    auto x = Register::b;
    auto shiftVal = Register::c;
    auto res = Register::e;
    auto shiftValNeg = Register::d;
    
    bool vNeg = false;
    auto v = ((ConstValue*)(this->right))->value;
    if (v < 0){
        v = -v;
        vNeg = true;
    }
    NumberType mask = 0b1;
    NumberType shifts = 0;
    while(mask <= v){
        mask <<= 1;
        shifts++;
    }

    shifts -= 1;
    auto inst = generateNumber(shifts, shiftVal);
    auto negShift = generateNumber(-shifts, shiftValNeg);
    auto xToReg = left->valueToRegister(x);
    
    auto constVal = generateNumber(v, res);
    constVal->push_back(new Instruction(OptCode::SWAP, x));
    constVal->push_back(new Instruction(OptCode::SWAP, res));
    constVal->push_back(new Instruction(OptCode::ADD, x));
    constVal->push_back(new Instruction(OptCode::SWAP, x));

    inst->splice(inst->end(), *(negShift));
    inst->splice(inst->end(), *(xToReg));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::ADD, x));
    inst->push_back(new Instruction(OptCode::SHIFT, shiftValNeg));
    inst->push_back(new Instruction(OptCode::SHIFT, shiftVal));
    inst->push_back(new Instruction(OptCode::SWAP, x));
    inst->push_back(new Instruction(OptCode::SUB, x));
    inst->push_back(new Instruction(OptCode::JNEG, 2));
    inst->push_back(new Instruction(OptCode::JUMP, constVal->size()+1));
    inst->splice(inst->end(), *(constVal));

    if(vNeg){
        inst->push_back(new Instruction(OptCode::SWAP, x));
        inst->push_back(new Instruction(OptCode::RESET, Register::a));
        inst->push_back(new Instruction(OptCode::SUB, x));
    }
    if(target != Register::a){
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }

    delete xToReg;
    delete negShift;
    delete constVal;
    return inst;

}
InstructionList* ModExpression::generateForNoConst(Register target){
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
    /*  sign = 0; pOne = 1;  mOne = -1; r = 0;*/
    inst->push_back(new Instruction(OptCode::RESET, sign)); // Block size = 7 | 51
    inst->push_back(new Instruction(OptCode::RESET, pOne));
    inst->push_back(new Instruction(OptCode::INC, pOne));
    inst->push_back(new Instruction(OptCode::RESET, mOne));
    inst->push_back(new Instruction(OptCode::DEC, mOne));
    inst->push_back(new Instruction(OptCode::RESET, r));
    /* if !(-a < 0) { a = -a; sign = 1;} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 6
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = -a 
    inst->push_back(new Instruction(OptCode::JNEG, 3)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, a)); // swap acc=-a with a
    inst->push_back(new Instruction(OptCode::INC, sign));
    /* if !(-b < 0) { b = -b; sign = sign + 2} */
    inst->push_back(new Instruction(OptCode::RESET, Register::a)); // Block size = 7 || 38
    inst->push_back(new Instruction(OptCode::SUB, b)); // acc = -b 
    inst->push_back(new Instruction(OptCode::JNEG, 4)); // if (acc < 0) jump out
    inst->push_back(new Instruction(OptCode::SWAP, b)); // swap acc=-b with b
    inst->push_back(new Instruction(OptCode::INC, sign)); //
    inst->push_back(new Instruction(OptCode::INC, sign)); //| sign += 2
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
    /*
        Sign:
        = 0 when a >= 0 and b >= 0: return 
        = 1 when a < 0 and b >0: res = b - res; 
        = 2 when a >= 0 and b < 0: res = 0 - res
        = 3 wgen a < 0 and b < 0: res = b - res and res = 0 - res
    */
    inst->push_back(new Instruction(OptCode::SWAP, sign));
    inst->push_back(new Instruction(OptCode::JZERO, 21)); // jump out
    /* Sign == 1 */
    inst->push_back(new Instruction(OptCode::DEC, Register::a));
    inst->push_back(new Instruction(OptCode::JPOS, 6)); 
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::ADD, d)); // acc = b 
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = b - r
    inst->push_back(new Instruction(OptCode::SWAP, a));// r = b-r
    inst->push_back(new Instruction(OptCode::JUMP, 14)); // jump out
    /* sign == 2 */
    inst->push_back(new Instruction(OptCode::DEC, Register::a));
    inst->push_back(new Instruction(OptCode::JPOS, 5)); 
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = 0 - r
    inst->push_back(new Instruction(OptCode::SWAP, a));// r = -r
    inst->push_back(new Instruction(OptCode::JUMP, 8)); // jump out
    /* sign == 3 */
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::ADD, d)); // acc = b 
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = b - r
    inst->push_back(new Instruction(OptCode::SWAP, a));
    inst->push_back(new Instruction(OptCode::RESET, Register::a));
    inst->push_back(new Instruction(OptCode::SUB, a)); // acc = 0 - r
    inst->push_back(new Instruction(OptCode::SWAP, a));
    /* Put result in desired register */
    if(target != a){
        inst->push_back(new Instruction(OptCode::SWAP, a));
        inst->push_back(new Instruction(OptCode::SWAP, target));
    }

    return inst;
}
