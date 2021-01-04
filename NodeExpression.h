#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>

enum Type { NONE = 0, VOID, INT, CHAR };

class FullType {
 public:
  Type type;

  bool const_expr = false;

  bool seq = false;

  bool tType = false;

  bool xType = false;

 private:
  auto tied() const { return std::tie(type, const_expr, seq, tType, xType); }

 public:
  FullType();
  FullType(Type);

  bool isTType();

  bool isXType();

  bool isConstTType();

  bool isSeqXType();

  bool isImplicitlyCastableToInt();

  bool isImplicitlyCastableToUnknownType(FullType);

  bool isExplicitlyCastable(FullType);

  bool isImplicitlyCastableToT();

  bool operator==(FullType const& other) const {
    return this->tied() == other.tied();
  }

  bool operator!=(FullType const& other) const {
    return this->tied() != other.tied();
  }
};

struct Parameter {
  FullType fullType;
  std::string name;
};

class Attributes {
 public:
  // expression attributes
  // Type type;

  bool l_expr = false;
  int elem_num = -1;

  // bool const_expr = false;

  FullType fullType;

  // function attributes
  std::vector<Parameter> parameters;  // type name
  FullType return_type;
  bool isFunction = false;
  bool defined = false;

  Attributes();
  Attributes(FullType, bool);
  Attributes(FullType);
};
