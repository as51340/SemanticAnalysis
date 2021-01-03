#include "Expressions.h"

//TODO  expression

bool IsCharCorrect(std::string lexUnit) {
    return true; //TODO
}

bool isSeqCharCorrect(std::string lexUnit) {
    return true; //TODO
}

Attributes PrimaryExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> child = node->children[0];
    if(child->grammarSign == "IDN") {
        try {
            Attributes atr = node->findScope(child->lexUnit); //if this command executes succesfully, variable exists in some scope, else we threw 
            //exception and we need to print error
            return atr;
        } catch(const std::invalid_argument&) {
            node->error();
        }
    } else if(child->grammarSign == "BROJ") {
        long long num = stoi(child->lexUnit); //we assume it fits into long long
        if(num < −2147483648 || num > 2147483647) { //value is not int
            node->error();
        } else {
            Attributes atr;
            atr.type = TYPE::INT;
            atr.l_expr = false;
            return atr;
        }
    } else if(child->grammarSign == "ZNAK") {
        if(IsCharCorrect(child->lexUnit)) {
            Attributes atr;
            atr.type = TYPE::CHAR;
            atr.l_expr = false;
            return atr;
        } else {
            node->error();
        }
    } else if(child->grammarSign == "NIZ_ZNAKOVA") {
        if(isSeqCharCorrect(child->lexUnit)) {
            Attributes atr;
            atr.seq = true;
            atr.const_expr = true;
            atr.type = TYPE::CHAR;
            atr.l_expr = false;
            return atr;
        } else {
        node->error();
        }
    } else if(child->grammarSign == "L_ZAGRADA") {
        Attributes expr_atr = Expression(node->children[1]);
        Attributes atr;
        atr.type = expr_atr.type;
        atr.l_expr = expr_atr.l_expr;
        return atr;
    }
    throw std::logic_error("Unknown first child in primary expression!");
}

Attributes PostfixExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<primarni_izraz>") {
        Attributes primary_atr = PrimaryExpression(firstChild);
        Attributes atr;
        atr.type = primary_atr.type;
        atr.l_expr = primary_atr.l_expr;
        return atr;
    } else if(firstChild->grammarSign == "<postfix_izraz>") {
        std::shared_ptr<Node> secondChild = node->children[1];
        if(secondChild->grammarSign == "L_UGL_ZAGRADA") {
            Attributes postfix_atr = PostfixExpression(firstChild);
        }
    }
    
}