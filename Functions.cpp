#include "Functions.h"

#include <functional>
#include <iostream>
#include <unordered_map>

#include "Expressions.h"
#include "NodeExpression.h"

extern std::shared_ptr<Node> root;
// auto& global_scope = root->local_scope; //iz nekog razloga ne radi

bool inside_loop, inside_function;
Attributes current_function_atr;

// prints the node production
void Error(std::shared_ptr<Node> node) {
  std::ofstream ostream("errors.out");
  ostream << node->grammarSign << " ::=";
  for (auto& child : node->children) {
    ostream << ' ' << child->grammarSign;
    if (child->isTerminal()) {
      ostream << '(' << child->rowNumber << ',' << child->lexUnit << ')';
    }
  }
  ostream << std::endl;
  exit(-1);  // we need to finish program
}

// Returns true if given parameters have the same types(basic types)
bool EqualTypes(std::vector<Parameter> a, std::vector<Parameter> b) {
  if (a.size() != b.size()) return false;
  for (int i = 0; i < a.size(); i++)
    if (a[i].fullType != b[i].fullType) return false;
  return true;
}

void CompilationUnit(std::shared_ptr<Node> node) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "prijevodna_jedinica>")
      CompilationUnit(child);
    else if (sign == "<vanjska_deklaracija>")
      OuterDeclaration(child);
  }
}

void OuterDeclaration(std::shared_ptr<Node> node) {
  auto& child = node->children[0];  // unary production
  std::string sign = child->grammarSign;

  if (sign == "<definicija_funkcije>")
    FunctionDefinition(child);
  else if (sign == "<deklaracija>")
    Declaration(child);
}

// zajebano
void FunctionDefinition(std::shared_ptr<Node> node) {
  Attributes return_atr;
  std::vector<Parameter> parameters;
  std::string function_name;

  bool old = inside_function;
  inside_function = true;
  Attributes old_atr = current_function_atr;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;
    // clang-format off
    if (sign == "<ime_tipa>") {
      return_atr = TypeName(child);
      if (return_atr.fullType.const_expr) Error(node);
    }
    else if (sign == "IDN") {
      function_name = child->lexUnit;
      if (root->local_scope.count(function_name) &&  // is declared
          root->local_scope[function_name].defined)
        Error(node);
    }
    else if (sign == "KR_VOID") {
      if (root->local_scope.count(function_name) &&
          (root->local_scope[function_name].return_type.type != return_atr.fullType.type ||
          !root->local_scope[function_name].parameters.empty()))
        Error(node);

      Attributes& declared_function = root->local_scope[function_name];
      declared_function.defined = true;
      declared_function.return_type.type = return_atr.fullType.type;
      declared_function.isFunction = true;
      current_function_atr = declared_function;
    }
    else if (sign == "<lista_parametara>") {
      parameters = ParameterList(child);
      if (root->local_scope.count(function_name) &&
          (root->local_scope[function_name].return_type.type != return_atr.fullType.type ||
          !EqualTypes(parameters, root->local_scope[function_name].parameters)))
        Error(node);

      Attributes& declared_function = root->local_scope[function_name];
      declared_function.defined = true;
      declared_function.return_type.type = return_atr.fullType.type;
      declared_function.parameters = parameters;
      declared_function.isFunction = true;
      current_function_atr = declared_function;
    }
    else if (sign == "<slozena_naredba>") ComplexCommand(child, parameters);
    // clang-format on
  }

  inside_function = old;
  current_function_atr = old_atr;
}

// type and const_expr needed
Attributes TypeName(std::shared_ptr<Node> node) {
  Attributes atr;
  Type specifier = Type::NONE;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;
    if (sign == "KR_CONST")
      atr.fullType.const_expr = true;
    else if (sign == "<specifikator_tipa>")
      specifier = TypeSpecifier(child);
  }
  if (atr.fullType.const_expr && specifier == Type::VOID) Error(node);

  atr.fullType.type = specifier;
  return atr;
}

std::vector<Parameter> ParameterList(std::shared_ptr<Node> node) {
  std::vector<Parameter> params, params_list;
  Parameter declared_param;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<lista_parametara>") {
      params_list = ParameterList(child);
      params.insert(params.end(), params_list.begin(), params_list.end());
    } else if (sign == "<deklaracija_parametra>") {
      declared_param = ParameterDeclaration(child);
      params.push_back(declared_param);
    }
  }

  // check if names are unique
  for (auto& param : params_list)
    if (param.name == declared_param.name) Error(node);

  return params;
}

Parameter ParameterDeclaration(std::shared_ptr<Node> node) {
  Parameter param;
  Attributes type_name_atr;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<ime_tipa>") {
      type_name_atr = TypeName(child);
      if (type_name_atr.fullType.type == Type::VOID) Error(child);
    } else if (sign == "IDN") {
      param.name = child->lexUnit;
    }
  }

  param.fullType = type_name_atr.fullType;
  if (node->children.size() > 2) param.fullType.seq = true;
  return param;
}

void ComplexCommand(std::shared_ptr<Node> node,
                    std::vector<Parameter> parameters = {}) {
  for (auto parameter : parameters) {
    auto& [full_type, name] = parameter;
    node->local_scope[name].fullType.type = full_type.type;
  }

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<lista_naredbi>")
      CommandList(child);
    else if (sign == "<lista_deklaracija>")
      DeclarationList(child);
  }
}

Type TypeSpecifier(std::shared_ptr<Node> node) {
  std::string sign = node->children[0]->grammarSign;

  if (sign == "KR_VOID") return Type::VOID;
  if (sign == "KR_CHAR") return Type::CHAR;
  if (sign == "KR_INT") return Type::INT;
  return Type::NONE;
}

void CommandList(std::shared_ptr<Node> node) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<naredba>")
      Command(child);
    else if (sign == "<lista_naredbi>")
      CommandList(child);
  }
}

FullType Command(std::shared_ptr<Node> node) {
  auto child = node->children[0];
  std::string sign = child->grammarSign;
  // clang-format off
  if (sign == "<slozena_naredba>") ComplexCommand(child);
  else if(sign == "<izraz_naredba>") return ExpressionCommand(child);
  else if(sign == "<naredba_grananja>") BranchingCommand(child);
  else if(sign == "<naredba_petlje>")  LoopCommand(child);
  else if(sign == "<naredba_skoka>")  JumpCommand(child);
  // clang-format on
  return {Type::NONE};  // ???
}

FullType ExpressionCommand(std::shared_ptr<Node> node) {
  auto child = node->children[0];

  if (child->grammarSign == "<izraz>") return Command(child);
  return {Type::INT};  // does this work?
}

void BranchingCommand(std::shared_ptr<Node> node) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<izraz>") {
      Attributes expr_atr = Expression(child);
      if (!expr_atr.fullType.isImplicitlyCastableToInt()) Error(node);
    } else if (sign == "<naredba>")
      Command(child);
  }
}

void LoopCommand(std::shared_ptr<Node> node) {
  int expr_command_cnt = 0;
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<izraz>") {
      Attributes expr_atr = Expression(child);
      if (node->children[0]->grammarSign == "KR_WHILE" &&
          !expr_atr.fullType.isImplicitlyCastableToInt())
        Error(node);
    } else if (sign == "<naredba>") {
      bool old = inside_loop;
      inside_loop = true;
      Command(child);
      inside_loop = old;
    } else if (sign == "<izraz_naredba>") {
      expr_command_cnt++;
      Attributes expr_command_atr = ExpressionCommand(child);
      if (expr_command_cnt > 1 &&  // == 2
          !expr_command_atr.fullType.isImplicitlyCastableToInt())
        Error(node);
    }
  }
}

void JumpCommand(std::shared_ptr<Node> node) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if ((sign == "KR_CONTINUE" || sign == "KR_BREAK") && !inside_loop)
      Error(node);

    if (sign == "KR_RETURN" && node->children.size() == 2 &&
        (!inside_function || current_function_atr.fullType.type != Type::VOID))
      Error(node);

    if (sign == "<izraz>") {
      Attributes expr_atr = Expression(child);
      if (!inside_function ||
          !expr_atr.fullType.isImplicitlyCastableToUnknownType(
              current_function_atr.fullType))
        Error(node);
    }
  }
}

void DeclarationList(std::shared_ptr<Node> node) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<deklaracija>")
      Declaration(child);
    else if (sign == "<lista_deklaracija>")
      DeclarationList(child);
  }
}

void Declaration(std::shared_ptr<Node> node) {
  Attributes type_name_atr;
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<ime_tipa>")
      type_name_atr = TypeName(child);
    else if (sign == "<lista_init_deklaratora>") {
      if (type_name_atr.isFunction)  // vjv ne treba
        InitDeclaratorList(child, type_name_atr.return_type);
      else
        InitDeclaratorList(child, type_name_atr.fullType);
    }
  }
}

void InitDeclaratorList(std::shared_ptr<Node> node, FullType inherited_type) {
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<init_deklarator>")
      InitDeclarator(child, inherited_type);
    else if (sign == "<lista_init_deklaratora>")
      InitDeclaratorList(child, inherited_type);
  }
}

// zajebano
// TODO: uljepsat uvjete
void InitDeclarator(std::shared_ptr<Node> node, FullType inherited_type) {
  Attributes initializer_atr, direct_atr;
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<izravni_deklarator>")
      direct_atr = DirectDeclarator(child, inherited_type);
    else if (sign == "<inicijalizator>")
      initializer_atr = Initializer(child);
  }

  FullType direct = direct_atr.fullType;
  if (node->children.size() == 1 && direct.isConstTType()) {
    Error(node);
  } else {  // tocka 3. str 69
    if (!direct.seq && direct.isXType()) {
      if (!initializer_atr.fullType.isImplicitlyCastableToT()) Error(node);
    } else if (direct.isSeqXType()) {
      if (initializer_atr.elem_num > direct_atr.elem_num) Error(node);
      for (auto& param : initializer_atr.parameters) {
        auto& [type, name] = param;
        if (!type.isImplicitlyCastableToT()) Error(node);
      }
    } else {
      Error(node);
    }
  }
}

// zajebano
Attributes DirectDeclarator(std::shared_ptr<Node> node,
                            FullType inherited_type) {
  Attributes atr;
  std::vector<Parameter> parameters;
  std::string name;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;
    // clang-format off
    if (sign == "IDN") {
      name = child->lexUnit;
      if (inherited_type.type == Type::VOID || node->local_scope.count(name)) Error(node);
      atr.fullType = inherited_type;
    }
    else if(sign == "BROJ"){
      int val = std::stoi(child->lexUnit);
      if (inherited_type.type == Type::VOID || node->local_scope.count(name)) Error(node);
      if(val < 0 || val > 1024) Error(child);
      atr.fullType = inherited_type;
      atr.fullType.seq = true;
      atr.elem_num = val;
    }
    else if (sign == "KR_VOID"){
      auto& local_scope = node->local_scope;

      if (local_scope.count(name)) { //if declared
        if (!(local_scope[name].isFunction &&
              local_scope[name].return_type == inherited_type &&
              local_scope[name].parameters.empty()))
          Error(node); // if already declared with different Attributes
      } else {  // declare
        atr.isFunction = true;
        atr.return_type = inherited_type;
      }
    }
    else if (sign == "<lista_parametara>") {
      parameters = ParameterList(child);

      auto& local_scope = node->local_scope;
      if (local_scope.count(name)) { //if declared
        if (!(local_scope[name].isFunction &&
              local_scope[name].return_type == inherited_type &&
              EqualTypes(local_scope[name].parameters, parameters)))
          Error(node); // if already declared with different Attributes
      } else {  // declare
        atr.isFunction = true;
        atr.return_type = inherited_type;
        atr.parameters = parameters;
      }
    }
    // clang-format on
  }

  node->local_scope[name] = atr;
  return atr;
}

Attributes Initializer(std::shared_ptr<Node> node) {
  Attributes atr, assignment_atr, assignment_list_atr;
  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<izraz_pridruzivanja>") {
      assignment_atr = AssignmentExpression(child);
      if (assignment_atr.elem_num != -1) {
        atr.elem_num = assignment_atr.elem_num + 1;
        atr.parameters = std::vector<Parameter>(atr.elem_num, {Type::CHAR});
      } else {
        atr.fullType = assignment_atr.fullType;
      }
    } else if (sign == "<lista_izraza_pridruzivanja>") {
      assignment_list_atr = AssignmentExpressionList(child);
      atr.elem_num = assignment_list_atr.elem_num;
      atr.parameters = assignment_list_atr.parameters;
    }
  }

  return atr;
}

Attributes AssignmentExpressionList(std::shared_ptr<Node> node) {
  Attributes atr, assignment_atr, assignment_list_atr;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;

    if (sign == "<lista_izraza_pridruzivanja>")
      assignment_list_atr = AssignmentExpressionList(child);
    else if (sign == "<izraz_pridruzivanja>")
      assignment_atr = AssignmentExpression(child);
  }

  if (node->children.size() != 1) {
    atr.parameters = assignment_list_atr.parameters;
    atr.elem_num = assignment_list_atr.elem_num + 1;
  } else {
    atr.elem_num = 1;
  }

  atr.parameters.push_back({assignment_atr.fullType});
  return atr;
}
