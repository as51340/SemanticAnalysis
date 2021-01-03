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

Attributes Node::findScope(std::string var) {
    std::shared_ptr<Node> currNode = std::make_shared<Node>(*this);
    while(true) {
        if(currNode->local_scope.count(var)) {
            return currNode->local_scope[var];
        }
        if(currNode->parent == nullptr) { //parent was met 
            break;
        } 
        currNode = currNode->parent; //move up
    }
    throw std::invalid_argument("Variable doesn't exist in any scope!");
}

// prints the node production
void Node::error() {
  std::cout << this->grammarSign << " ::=";
  for (auto& child : this->children) {
    std::cout << ' ' << this->grammarSign;
    if (child->isTerminal())
      std::cout << '(' << child->rowNumber << ',' << child->lexUnit << ')';
  }
  std::cout << std::endl;
  exit(-1); //we need to finish program
}


