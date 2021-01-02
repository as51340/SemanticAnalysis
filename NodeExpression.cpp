#include "NodeExpression.h"


NodeExpression::NodeExpression(){};
NodeExpression::NodeExpression(std::string type, bool l_express): type(type), l_express(l_express) {};


void NodeExpression::setTypes(std::vector<std::string> types) {
    for(std::string type: types) {
        this->types.push_back(std::make_shared<std::string>(std::move(type)));
    }
}

void NodeExpression::setNames(std::vector<std::string> names) {
    for(std::string name: names) {
        this->names.push_back(std::make_shared<std::string>(std::move(name)));
    }
    
}