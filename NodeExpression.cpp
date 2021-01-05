#include "NodeExpression.h"

// clang-format off
Attributes::Attributes(){};
Attributes::Attributes(FullType fullType, bool l_expr) : fullType(fullType), l_expr(l_expr){};
Attributes::Attributes(FullType fullType) : fullType(fullType) {};

FullType::FullType() : type(Type::NONE) {};
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
    if((const_expr == true && type == Type::INT) || type == Type::CHAR  || type == Type::INT) {
        return true;
    }
    return false;
}

// upitno
bool FullType::isImplicitlyCastableToT(){
    //std::cerr << "Seq: " << this->seq << " isTType" << this->isTType() << " isConstTType" << isConstTType() << std::endl;
    if(this->isTType() && this->seq == false) return true;
    if(this->isConstTType() && this->seq == false) return true;
    return false;
}


bool FullType::isImplicitlyCastableToUnknownType(FullType fullType) {
    if(fullType.seq != seq)  {
        //std::cerr << "TU" << std::endl;
        return false;
    }
    if(fullType.type == Type::VOID) {
        return false;
    }
    if(type == fullType.type) {
        return true;
    }
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
    if(isImplicitlyCastableToUnknownType(fullType)) return true;
    if(fullType.type == Type::CHAR && type == Type::INT) return true;
    return false;
}
