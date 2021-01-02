#pragma once
#include <vector>
#include <memory> //shared_ptr when last pointer to the object is reassigned or deleted then is object deleted
#include <unordered_map>
#include <string>
#include <fstream>
#include "NodeExpression.h"


class Node {
    
public:
    
    //syntax properties
    std::string lexUnit = ""; //not assigned
    long rowNumber = -1; //not assigned
    std::string grammarSign = ""; //not assigned, na temelju toga odlucujes kako dalje
    
    //semantic properties
    std::vector<std::shared_ptr<Node>> children;
    std::shared_ptr<Node> parent = nullptr;
    std::unordered_map<std::string, NodeExpression> local;
    
    
    
    Node();
    Node(std::string, std::string, long);
    Node(std::string);
    
    
    bool isTerminal();
    
    void addChild(std::shared_ptr<Node>);
    
    void printNode(std::ostream&);
    
    
};