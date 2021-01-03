#include "NodeExpression.h"

Attributes::Attributes(){};
Attributes::Attributes(FullType type, bool l_expr) : type(type), l_expr(l_expr){};
Attributes(FullType type) : type(type) {};

FullType::FullType() : type(TYPE::NONE) {};
FullType::FullType(Type type) :type(type) {};


bool FullType::isTType() {
    if(tType) return true; //vec je postavljen
    if(type == Type::INT || type == Type::CHAR) {
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
    if(seq) { //vec je postavljen
        return isXType();
    }
    return false;
}

bool FullType::isImplicitlyCastableToInt() {
    if((const_expr == true && type == Type::INT) || type == Type::CHAR) {
        return true;
    }
    return false;
}


bool FullType::isImplicitlyCastableToUnknownType(FullType fullType) {
    if(fullType.type == Type::INT) {
        return isImplicitlyCastableToInt();
    }
    if(isConstTType() && fullType.isTType() && type == fullType.type) {
        return true;
    }
    if(isTType() && fullType.isConstTType() && type == fullType.type) {
        return true;
    }
    if(seq && isTType() && !isConstTType() && fullType.seq && fullType.isConstTType() && type == fullType.type) {
        return true;
    }
    return false;
}

bool FullType::isExplicitlyCastable(FullType fullType) { //pitaj boga da li je ovo dobro
    if(type != Type::INT || fullType.type != Type::CHAR) {
        return false;
    }
    //we do not handle NA for casting
    return true;
}
 