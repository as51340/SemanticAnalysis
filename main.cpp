#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "Tree.h"
#include "Node.h"

std::shared_ptr<Tree> tree = std::make_shared<Tree>();


void createNewNode(std::shared_ptr<Node> newNode, std::shared_ptr<Node> currNode) {
    if(!tree->isCreated) {
        tree->root = std::make_shared<Node>(*newNode); //number of refs incremented        
        tree->isCreated = true;
    } else {
        currNode->addChild(std::make_shared<Node>(*newNode));
        newNode->parent = currNode;
    }
    currNode = std::make_shared<Node>(*newNode); 
}


void parseInput() {
    std::string line;
    std::shared_ptr<Node> currNode = std::make_shared<Node>();  //one object allocated
    while(std::getline(std::cin, line)) {
        //std::cout << "Procitana linija: " << line << std::endl;
        std::istringstream line_stream(line);
        std::string item;
        std::vector<std::string> elements;
        while(line_stream >> item) {
            elements.push_back(item);
        }
        std::shared_ptr<Node> newNode = std::make_shared<Node>(); //new object
        if(elements.size() == 1) {
            if(elements[0].compare("$") == 0) {
                //std::cerr << "Radim novi epsilon znak\n";
                newNode->grammarSign = "$";
            } else  {
               // std::cerr << "Radim novi nezavrsni znak\n";
                newNode->grammarSign = elements[0];
            }
        } else if(elements.size() == 3) {
            //std::cerr << "Radim novi zavrsni znak!" << std::endl;
            newNode->grammarSign = elements[0];
            newNode->rowNumber = stoi(elements[1]);
            newNode->lexUnit = elements[2];
        } else {
            throw std::invalid_argument("Parser ne dela dobro!\n");
        }
        createNewNode(newNode, currNode);
    }
}

int main(void) {
    parseInput();
    tree->printTree();
    return 0;
}