#include "Node.h"


Node::Node() {};
Node::Node(std::string grammarSign, std::string lexUnit, long rowNumber): grammarSign(grammarSign), lexUnit(lexUnit), rowNumber(rowNumber) {};
Node::Node(std::string grammarSign): grammarSign(grammarSign) {};


bool Node::isTerminal() {
    return rowNumber != -1;
}
 
void Node::addChild(std::shared_ptr<Node> child) {
    this->children.push_back(child);
}

void Node::printNode(std::ostream& ofStream) {
    if(grammarSign == "$") {
        ofStream << "$" << std::endl;
    } else if(rowNumber != -1) { //terminal
        ofStream << grammarSign << " " << rowNumber << " " << lexUnit << std::endl;
    } else { //not terminal
        ofStream << grammarSign << std::endl;
    }
}


