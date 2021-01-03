#include "Tree.h"

std::ofstream ostream("output.out");

void printRecursively(std::shared_ptr<Node> root, short spaces);


Tree::Tree() {};

Tree::Tree(std::shared_ptr<Node> root) {
    this->root = root;
}

void Tree::printTree() {
    printRecursively(root, 0);
}

void printRecursively(std::shared_ptr<Node> root, short spaces) {    
    //std::cerr << std::string(spaces, ' ');
    ostream << std::string(spaces, ' ');
    //root->printNode(std::cerr);
    root->printNode(ostream);
    for(std::shared_ptr<Node> child: root->children) {
        printRecursively(child, spaces + 1);
    }
}