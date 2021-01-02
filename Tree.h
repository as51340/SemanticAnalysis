#pragma once
#include <memory>
#include "Node.h"

class Tree {
    
public:

    std::shared_ptr<Node> root;
    
    Tree();
    Tree(Node root);
    
};