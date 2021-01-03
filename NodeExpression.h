#pragma once
#include <memory>
#include <string>
#include <vector>

enum Type { NONE = 0, VOID, INT, CHAR };

struct Parameter {
  Type type;
  std::string name;
};

class Attributes {
 public:
  // expression attributes
  Type type;
  bool l_expr = false;

  bool const_expr = false;

  // function attributes
  std::vector<Parameter> parameters;  // type,name
  Type return_type;
  bool defined = false;

  Attributes();
  Attributes(Type, bool);
};
