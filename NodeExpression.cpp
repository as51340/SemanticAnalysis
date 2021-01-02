#include "NodeExpression.h"


NodeExpression::NodeExpression(){};
NodeExpression::NodeExpression(std::string type, bool l_express): type(type), l_express(l_express) {};


void NodeExpression::setTypes(std::vector<std::string>& types) {
    this->types = types;
}

void NodeExpression::setNames(std::vector<std::string>& names) {
    this->names = names;
}