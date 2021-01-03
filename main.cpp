#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "Tree.h"
#include "Node.h"
#include <cctype>

std::shared_ptr<Tree> tree = std::make_shared<Tree>();
std::shared_ptr<Node> root = tree->root;

void assign(std::string line, std::shared_ptr<Node> newNode) {    
    std::istringstream line_stream(line);
    std::string item;
    std::vector<std::string> elements;
    while(line_stream >> item) {
        elements.push_back(item);
    }
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
}

std::string recursiveReading(std::shared_ptr<Node> root, short spaces) {
    std::string line;
    if(!std::getline(std::cin, line)) return "";
    while(true) {
        if(isspace(line[spaces]) && !isspace(line[spaces+1]) && ((spaces > 0 && isspace(line[spaces-1])) || spaces == 0)) { // to mu je dijete
            std::shared_ptr<Node> newNode = std::make_shared<Node>(); //new object
            assign(line, newNode);
            std::cout << newNode->grammarSign << std::endl;
            root->addChild(newNode);
            newNode->parent = root;
            std::string ret = recursiveReading(newNode, spaces+1);
            if(ret == "") return ret;
            line = ret;
        } else {
            break;
        }
    }
    return line;
}


void parseInput() {
    std::string line;
    std::getline(std::cin, line);
    std::shared_ptr<Node> newNode = std::make_shared<Node>();
    assign(line, newNode);
    tree->root = newNode;
    recursiveReading(tree->root, 0);
}

int main(void) {
    parseInput();
    tree->printTree();
    return 0;
}