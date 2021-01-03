#include "NodeExpression.h"

Attributes::Attributes(){};
Attributes::Attributes(FullType type, bool l_expr) : type(type), l_expr(l_expr){};
Attributes(FullType type) : type(type) {};

FullType::FullType() : type(TYPE::NONE) {};
FullType::FullType(Type type) :type(type) {};


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


bool FullType::isImplicitlyCastableToUnknownType(FullType fullType) {
    if(fullType.type == TYPE::INT) {
        return isImplicitlyCastableToInt();
    }
    if(isConstTType() && fullType.isTType()) {
        return true;
    }
    if(isTType() && fullType.isConstTType()) {
        return true;
    }
    if(seq == true && isTType() && !isConstTType() && fullType.seq == true && fullType.isConstTType()) {
        return true;
    }
    return false;
}

bool FullType::isExplicitlyCastable(FullType fullType) { //pitaj boga da li je ovo dobro
    if(isConstTType()) {
        if(fullType.isConstTType()) {
            return true;
        }
        return false;
    } return false;
}
 