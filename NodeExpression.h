#pragma once
#include <memory>
#include <string>
#include <vector>

enum Type { NONE = 0, VOID, INT, CHAR };

struct Parameter {
  FullType type;
  std::string name;
};

class FullType {
public:
    Type type;
    
    bool const_expr = false;
  
    bool seq = false;
    
    bool tType = false;
    
    bool xType = false;
    
    FullType();
    FullType(Type);
    
    bool isTType();
    
    bool isXType();
    
    bool isConstTType();
    
    bool isSeqXType();
    
    bool isImplicitlyCastableToInt();
    
    bool isImplicitlyCastableToUnknownType(FullType);
    
    bool isExplicitlyCastable(FullType);
    
};

class Attributes {
 public:
  // expression attributes
  //Type type;
  
  bool l_expr = false;

  //bool const_expr = false;
  
  FullType type;

  // function attributes
  std::vector<Parameter> parameters;  // type name
  FullType return_type;
  bool isFunction = false;
  bool defined = false;



  Attributes();
  Attributes(FullType, bool);
};



