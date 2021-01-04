#include <memory>

#include "Node.h"
#include "NodeExpression.h"

// RECURSIVE-DESCENT FUNCTIONS
// We rely on the assumption that the input syntax tree will always be
// syntactically correct (no surprise grammar symbols in productions)

void CompilationUnit(std::shared_ptr<Node>);
void OuterDeclaration(std::shared_ptr<Node>);
void FunctionDefinition(std::shared_ptr<Node>);
void Declaration(std::shared_ptr<Node>);     // variable or function declaration
Attributes TypeName(std::shared_ptr<Node>);  // also used in expressions
void ComplexCommand(std::shared_ptr<Node>, std::vector<Parameter>);
std::vector<Parameter> ParameterList(std::shared_ptr<Node>);
void CommandList(std::shared_ptr<Node>);
void DeclarationList(std::shared_ptr<Node>);
Type TypeSpecifier(std::shared_ptr<Node>);
Parameter ParameterDeclaration(std::shared_ptr<Node>);
FullType Command(std::shared_ptr<Node>);
FullType ExpressionCommand(std::shared_ptr<Node>);
void BranchingCommand(std::shared_ptr<Node>);
void LoopCommand(std::shared_ptr<Node>);
void JumpCommand(std::shared_ptr<Node>);
void InitDeclaratorList(std::shared_ptr<Node>, FullType);
void InitDeclarator(std::shared_ptr<Node>, FullType);
Attributes DirectDeclarator(std::shared_ptr<Node>, FullType);
Attributes Initializer(std::shared_ptr<Node>);
Attributes AssignmentExpressionList(std::shared_ptr<Node>);
