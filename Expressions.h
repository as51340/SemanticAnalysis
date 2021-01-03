#pragma once
#include <memory>
#include "Node.h"
#include "NodeExpression.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

Attributes PrimaryExpression(std::shared_ptr<Node>);

char charIsCorrect(std::string lexUnit);
std::string isSeqCharCorrect(std::string lexUnit);
Attributes PrimaryExpression(std::shared_ptr<Node> node);
bool checkImplicitCasting(std::vector<Parameter> types1, std::vector<Parameter> types2);
Attributes PostfixExpression(std::shared_ptr<Node> node);
Attributes ListArguments(std::shared_ptr<Node> node);
Attributes UnaryExpression(std::shared_ptr<Node> node);
Attributes CastExpression(std::shared_ptr<Node> node);
Attributes MultiplicativeExpression(std::shared_ptr<Node> node);
Attributes AditiveExpression(std::shared_ptr<Node> node);
Attributes RelationExpression(std::shared_ptr<Node> node);
Attributes EqualityExpression(std::shared_ptr<Node> node);
Attributes BitAndExpression(std::shared_ptr<Node> node);
Attributes BitXorExpression(std::shared_ptr<Node> node);
Attributes BitOrExpression(std::shared_ptr<Node> node);
Attributes LogAndExpression(std::shared_ptr<Node> node);
Attributes LogOrExpression(std::shared_ptr<Node> node);
Attributes AssignmentExpression(std::shared_ptr<Node> node);
Attributes Expression(std::shared_ptr<Node> node);
Attributes TypeName(std::shared_ptr<Node>); //also in expressions