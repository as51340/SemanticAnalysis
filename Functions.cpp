#include "Functions.h"

#include <functional>
#include <iostream>
#include <unordered_map>

#include "NodeExpression.h"

extern std::shared_ptr<Node> root;
auto& global_scope = root -> local_scope;

// prints the node production
void Error(std::shared_ptr<Node> node) {
  std::cout << node->grammarSign << " ::=";
  for (auto& child : node->children) {
    std::cout << ' ' << child->grammarSign;
    if (child->isTerminal())
      std::cout << '(' << child->rowNumber << ',' << child->lexUnit << ')';
  }
  std::cout << std::endl;
}

// Returns true if given parameters have the same types
bool EqualTypes(std::vector<Parameter> a, std::vector<Parameter> b) {
  if (a.size() != b.size()) return false;
  for (int i = 0; i < a.size(); i++)
    if (a[i].type != b[i].type) return false;
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

void FunctionDefinition(std::shared_ptr<Node> node) {
  Attributes return_atr;
  std::vector<Parameter> parameters;
  std::string function_name;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;
    // clang-format off
    if (sign == "<ime_tipa>") {
      return_atr = TypeName(child);
      if (return_atr.const_expr) Error(node);
    }
    else if (sign == "IDN") {
      function_name = child->lexUnit;
      if (global_scope.count(function_name) &&  // is declared
          global_scope[function_name].defined)
        Error(node);
    }
    else if (sign == "KR_VOID") {
      Attributes declared_function = global_scope[function_name];
      if (declared_function.return_type != return_atr.type ||
          !declared_function.parameters.empty())
        Error(node);

      declared_function.defined = true;
      declared_function.return_type = return_atr.type;
    }
    else if (sign == "<lista_parametara>") {
      parameters = ParameterList(child);
      Attributes declared_function = global_scope[function_name];
      if (declared_function.return_type != return_atr.type ||
          !EqualTypes(parameters, declared_function.parameters))
        Error(node);

      declared_function.defined = true;
      declared_function.return_type = return_atr.type;
      declared_function.parameters = parameters;
    }
    else if (sign == "<slozena_naredba>")
      ComplexCommand(child, parameters);
    // clang-format on
  }
}

// type and const_expr needed
// TODO: structs: (tip, l_expr), (tip, const_expr)
Attributes TypeName(std::shared_ptr<Node> node) {
  Attributes atr;
  Type specifier = Type::NONE;

  for (auto& child : node->children) {
    std::string sign = child->grammarSign;
    if (sign == "KR_CONST")
      atr.const_expr = true;
    else if (sign == "<specifikator_tipa>")
      specifier = TypeSpecifier(child);
  }

  if (atr.const_expr && specifier == Type::VOID) Error(node);

  atr.type = specifier;
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

  // TODO
  // for (auto& child : node->children) {
  //   std::string sign = child->grammarSign;

  //   if (sign == "<ime_tipa>") {
  //     TypeName(child);
  //   }
  // }

  return param;
}

void ComplexCommand(std::shared_ptr<Node> node,
                    std::vector<Parameter> parameters) {
  for (auto parameter : parameters) {
    auto& [type, name] = parameter;
    node->local_scope[name].type = type;
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
