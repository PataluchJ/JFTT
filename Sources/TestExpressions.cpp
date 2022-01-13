#include "Expressions.hpp"
#include "Identifiers.hpp"

int main(){
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


    return 0;
}