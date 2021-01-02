#include "Node.h"


Node::Node() {};
Node::Node(std::string unifSign, std::string lexUnit, long rowNumber): unifSign(unifSign), lexUnit(lexUnit), rowNumber(rowNumber) {};
Node::Node(std::string grammarSign): grammarSign(grammarSign) {};


bool Node::isTerminal() {
    return grammarSign != "";
}
    
void Node::addAllChildren(std::vector<Node> children) {
    for(Node child: children) {
        this->children.push_back(std::make_shared<Node>(std::move(child)));
    }
}
    
void Node::addChild(Node child) {
    this->children.push_back(std::make_shared<Node>(std::move(child)));
    
}

void Node::addEpsilonChild() {
    Node node;
    node.unifSign = "$";
    std::shared_ptr<Node> child = std::make_shared<Node>(std::move(node));
}


void Node::printNode(std::ostream& ofStream) {
    if(unifSign == "$") {
        ofStream << "$" << std::endl;
    } else if(grammarSign == "") {
        ofStream << unifSign << " " << rowNumber << " " << lexUnit << std::endl;
    } else {
        ofStream << grammarSign << std::endl;
    }
}
