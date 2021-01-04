#pragma once
#include "NodeExpression.h"
#include <fstream>
#include <memory> //shared_ptr when last pointer to the object is reassigned or deleted then is object deleted
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
public:
  // syntax properties
  std::string lexUnit = ""; // not assigned
  long rowNumber = -1;      // not assigned
  std::string grammarSign =
      ""; // not assigned, na temelju toga odlucujes kako dalje

  // semantic properties
  std::vector<std::shared_ptr<Node>> children;
  std::shared_ptr<Node> parent = nullptr;
  std::unordered_map<std::string, Attributes> local_scope;
  std::unordered_map<std::string, Attributes> declared_functions;

  Attributes findScope(std::string);

  std::shared_ptr<Node> getClosestScope();

  Node();
  Node(std::string, std::string, long);
  Node(std::string);

  bool isTerminal();

  void addChild(std::shared_ptr<Node>);

  void printNode(std::ostream &);

  void error();
};
