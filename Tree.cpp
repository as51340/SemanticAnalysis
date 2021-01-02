#include "Tree.h"

Tree::Tree() {};

Tree::Tree(Node& root) {
    
    this->root = std::make_shared<Node>(std::move(root));
    
}