#include "../Includes/Tester.hpp"

void Tester::testAssembly()
{
    std::cout << "Assembly test.\n";
    InstructionList* list = new InstructionList();
    list->push_back(new Instruction(OptCode::GET));
    list->push_back(new Instruction(OptCode::PUT));
    list->push_back(new Instruction(OptCode::LOAD, Register::a));
    list->push_back(new Instruction(OptCode::STORE, Register::b));
    list->push_back(new Instruction(OptCode::ADD, Register::c));
    list->push_back(new Instruction(OptCode::SUB, Register::d));
    list->push_back(new Instruction(OptCode::SHIFT, Register::e));
    list->push_back(new Instruction(OptCode::RESET, Register::f));
    list->push_back(new Instruction(OptCode::INC, Register::g));
    list->push_back(new Instruction(OptCode::DEC, Register::a));
    list->push_back(new Instruction(OptCode::JUMP, 69));
    list->push_back(new Instruction(OptCode::JPOS, 69));
    list->push_back(new Instruction(OptCode::JZERO, 69));
    list->push_back(new Instruction(OptCode::JNEG, 69));
    list->push_back(new Instruction(OptCode::HALT));
    std::cout << "All asm instruction pushed to instruction list.\nPrinting list...\n";
    for (auto i : *list) {
        std::cout << i->toString() << "\n";
    }
    std::cout << "Test end.\n";

    std::cout << "Testing generatiung number: 1001110000110101010\n";
    long long n = 21;
    auto num = generateNumber(n, Register::a);
    for (auto i : *num) {
        std::cout << i->toString() << "\n";
    }
    std::cout << "Testing neg number -21:\n";
    long long p = -21;
    auto n2 = generateNumber(p, Register::b);
    for (auto i : *n2) {
        std::cout << i->toString() << "\n";
    }
}

void Tester::testEnvironment()
{
    std::cout << "Testing variable definitions\n";
    std::cout << "Declaring variables n, array[0:2], p\n";
    Environment::declareVariable("n");
    Environment::declareArray("array", -2, 3);
    Environment::declareVariable("p");
    std::cout << "n:\n";
    Util::printMemVar(Environment::getVariable("n"));
    std::cout << "array:\n";
    Util::printMemVar(Environment::getVariable("array"));
    std::cout << "p:\n";
    Util::printMemVar(Environment::getVariable("p"));
    std::cout << "Trying to access not existend variable:\n";
    Util::printMemVar(Environment::getVariable("none"));
    std::cout << "Redefining p\n";
    Environment::declareVariable("p");
    Util::printMemVar(Environment::getVariable("p"));
    std::cout << "Defining array with wrong indexes:\n";
    Environment::declareArray("wrong", 20, 10);
    Util::printMemVar(Environment::getVariable("wrong"));

    std::cout << "Testing labels\n";
    InstructionList* l1 = new InstructionList;
    InstructionList* l2 = new InstructionList;

    auto labeled1 = new Instruction(OptCode::LOAD, Register::b);
    auto labeled2 = new Instruction(OptCode::STORE, Register::b);

    l1->push_back(new Instruction(OptCode::GET));
    l1->push_back(labeled1);
    l1->push_back(new Instruction(OptCode::INC, Register::b));
    l2->push_back(new Instruction(OptCode::PUT));
    l2->push_back(labeled2);
    l2->push_back(new Instruction(OptCode::HALT));

    size_t lab1 = Environment::laberInstruction(labeled1);
    size_t lab2 = Environment::laberInstruction(labeled2);

    InstructionList* inst = new InstructionList;
    inst->merge(*l1);
    inst->merge(*l2);

    std::cout << "Not modyfied instruction list:\n";
    Util::printList(inst);
    Environment::getLabeledInstruction(lab1)->argument.reg = Register::c;
    std::cout << "Modified instruction list:\n";
    Environment::getLabeledInstruction(lab2)->argument.reg = Register::c;
    Util::printList(inst);
}

void Tester::testExpressions()
{
    std::string n_name = "n";
    std::string p_name = "p";

    Environment::declareVariable("n");
    Environment::declareVariable("p");

    Identifier* n_iden = new Variable(&n_name);
    Identifier* p_iden = new Variable(&p_name);

    Value* con1 = new ConstValue(15);
    Value* con2 = new ConstValue(3);
    Value* one = new ConstValue(1);
    Value* var1 = new VarValue(n_iden);
    Value* var2 = new VarValue(p_iden);

    // Add tests
    Expression* addExpCon = new AddExpression(con1, con2);
    Expression* addExpVar = new AddExpression(var1, var2);
    Expression* addExpOne = new AddExpression(var1, one);
}

void Tester::testIdentifiers()
{
    Environment::declareVariable("n");
    Environment::declareArray("array", -5, 5);
    std::string n_name = "n";
    std::string array_name = "array";
    Identifier* var = new Variable(&n_name);
    Identifier* arrConst = new ArrayByConst(&array_name, 2);
    Identifier* arrVar = new ArrayByVariable(&array_name, &n_name);

    auto varInst = var->addressToRegister(Register::a);
    auto constInst = arrConst->addressToRegister(Register::a);
    auto varArInst = arrVar->addressToRegister(Register::c);

    std::cout << "Variable to register a:\n";
    Util::printList(varInst);
    std::cout << "Array by const to register a:\n";
    Util::printList(constInst);
    std::cout << "Array by variable to register c:\n";
    Util::printList(varArInst);

    deleteInstructionList(varInst);
    deleteInstructionList(constInst);
    deleteInstructionList(varArInst);
}

void Tester::testAssign()
{
    Environment::declareVariable("n");
    std::string name = "n";
    Identifier* var = new Variable(&name);
    Value* val = new ConstValue(20);
    Expression* exp = new ConstExpression(val);
    Assign* ass = new Assign(var, exp);

    Value* varval = new VarValue(var);
    Value* coval = new ConstValue(300);

    Expression* exp2 = new AddExpression(varval, coval);
    Assign* sec = new Assign(var, exp2);

    auto list = ass->generate();
    auto list2 = sec->generate();

    
    Util::printList(list);
    Util::printList(list2);

}

void Tester::testAssignArray()
{
    Environment::declareArray("array", -10, 20);
    std::string arrayName = "array";
  //  Identifier* arrayIdentifier = new ArrayByConst("array", 0);
}
