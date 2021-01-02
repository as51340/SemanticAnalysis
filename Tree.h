#pragma once
#include <memory>
#include "Node.h"
#include <iostream>
#include <fstream>

class Tree {
    
public:

    std::shared_ptr<Node> root;
    bool isCreated = false;
    
    Tree();
    Tree(std::shared_ptr<Node>);
    
    void printTree();    
    
};