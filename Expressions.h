#pragma once
#include <memory>
#include "Node.h"
#include "NodeExpression.h"
#include <string>

Attributes PrimaryExpression(std::shared_ptr<Node>);

bool charIsCorrect(std::string lexUnit);