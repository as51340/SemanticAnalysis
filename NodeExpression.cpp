#include "NodeExpression.h"

Attributes::Attributes(){};
Attributes::Attributes(Type type, bool l_expr) : type(type), l_expr(l_expr){};


bool FullType::isTType() {
    if(tType) return true;
    if(type == TYPE::INT || type == TYPE::CHAR) {
        return true;
    }
    return false;
}

bool FullType::isConstTType() {
    if(const_expr) {
        return isTType();
    }
    return false;
}


bool FullType::isXType() {
    if(xType) return true;
    if(isTType() || isConstTType()) {
        return true;
    } 
    return false;
}

bool FullType::isSeqXType() {
    if(seq) {
        return isXType();
    }
    return false;
}

bool FullType::isImplicitlyCastableToInt() {
    if((const_expr == true && type == TYPE::INT) || type == TYPE::CHAR) {
        return true;
    }
    return false;
}