#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "Functions.h"
#include "Node.h"
#include "Tree.h"

// clang-format off
std::shared_ptr<Tree> tree = std::make_shared<Tree>();
std::shared_ptr<Node> root;

void assign(std::string line, std::shared_ptr<Node> newNode) {    
    std::istringstream line_stream(line);
    std::string item;
    std::vector<std::string> elements;
    int i = 0;
    while(line_stream >> item && i < 2) {
        i++;
        elements.push_back(item);
    }
    if(elements.size() == 2) {
        std::size_t found = line.find('\"');
        if(found != std::string::npos && line.find('\"', found+1) != std::string::npos) {
            elements.push_back(line.substr(found));
        } else {
            line_stream >> item;
            elements.push_back(item);
        }
    }
    
    if(elements.size() == 1) {
        if(elements[0].compare("$") == 0) {
            //std::cerr << "Radim novi epsilon znak\n";
            newNode->grammarSign = "$";
        } else {
            // std::cerr << "Radim novi nezavrsni znak\n";
            newNode->grammarSign = elements[0];
        }
    } else if(elements.size() == 3) {
        //std::cerr << "Radim novi zavrsni znak!" << std::endl;
        newNode->grammarSign = elements[0];
        newNode->rowNumber = stoi(elements[1]);
        newNode->lexUnit = elements[2];
    }  else {
        std::cerr << elements.size() << std::endl;
        for(int i = 0; i < elements.size(); i++) {
            std::cerr << elements[i] << "      ";
        }
        throw std::invalid_argument("Parser ne dela dobro!\n");
    }
}

std::string recursiveReading(std::shared_ptr<Node> root, short spaces) {
    std::string line;
    if(!std::getline(std::cin, line)) return "";
    while(true) {
        if(spaces < line.size() && isspace(line[spaces]) && !isspace(line[spaces+1]) && ((spaces > 0 && isspace(line[spaces-1])) || spaces == 0)) { // to mu je dijete
            std::shared_ptr<Node> newNode = std::make_shared<Node>(); //new object
            assign(line, newNode);
            //std::cout << newNode->grammarSign << std::endl;
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
    root = tree->root;
    recursiveReading(tree->root, 0);
}

// clang-format on
extern std::unordered_set<std::string> function_declarations;
extern bool EqualTypes(std::vector<Parameter> a, std::vector<Parameter> b);

std::ofstream ostream2("errors.out");
void dfs(std::shared_ptr<Node> node) {

  for (auto &pair : node->declared_functions) {
    Attributes declared = pair.second;
    Attributes defined_f = root->local_scope[pair.first];

    if (!defined_f.defined || declared.return_type != defined_f.return_type ||
        !EqualTypes(declared.parameters, defined_f.parameters)) {
      ostream2 << "funkcija" << std::endl;
      return;
    }
  }

  for (auto child : node->children)
    dfs(child);
}

int main(void) {
  parseInput();
  tree->printTree();
  // std::cerr << "P A R S E D\n" << std::endl;

  CompilationUnit(root);

  std::ofstream ostream("errors.out");
  if (root->local_scope.count("main")) {
    auto &main_atr = root->local_scope["main"];

    if (main_atr.return_type.type != Type::INT ||
        main_atr.return_type.const_expr == true ||
        main_atr.return_type.seq == true || !main_atr.parameters.empty()) {
      ostream << "main" << std::endl; // bad main type
      return 0;
    }
  } else {
    ostream << "main" << std::endl; // main not declared
    return 0;
  }

  dfs(root);

  return 0;
}
