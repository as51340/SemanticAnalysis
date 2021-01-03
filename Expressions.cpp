#include "Expressions.h"


char IsCharCorrect(std::string lexUnit) {
    if(lexUnit[0] == '\'') { //remove simple quotation marks '
        lexUnit = lexUnit.substr(1, lexUnit.size() - 2);
    }
    if(lexUnit.size() == 1 && lexUnit[0] == '\\') {
        return lexUnit[0];
    }
    if(lexUnit == "\\t") {
        return '\t';
    } else if(lexUnit == "\\n") {
        return '\n';
    } else if(lexUnit == "\\0") {
        return '\0';
    } else if(lexUnit == "\\'") {
        return '\t';
    } else if(lexUnit == "\\\"") {
        return '\"';
    } else if(lexUnit == "\\\\") {
        return '\\';
    }
    throw std::invalid_argument("Char is correct error!");
}

std::string isSeqCharCorrect(std::string lexUnit) {
    lexUnit = lexUnit.substr(1, lexUnit.size() - 2); //remove "
    std::string builder;
    int size = lexUnit.size();
    for(int i = 0; i < size; i++) {
        if(lexUnit[i] == '"') {
            throw std::invalid_argument("Sequence is not correct, \" exists");
        }
        if(lexUnit[i] == '\\') {
            if(i >= size -1) {
                throw std::invalid_argument("Sequence is not correct, cannot escape!");
            }
            try {
                char c = IsCharCorrect(lexUnit.substr(i, 2)); //check next char also
                i++;    
                builder += c; 
            } catch(const std::exception&) {
                throw std::invalid_argument("Error in escaping in char sequence!");
            }
        } else {
            builder += lexUnit[i];
        }
    }
    return builder;
}


Attributes PrimaryExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> child = node->children[0];
    if(child->grammarSign == "IDN") {
        try {
            return node->findScope(child->lexUnit); //if this command executes succesfully, variable exists in some scope, else we threw 
            //exception and we need to print error
        } catch(const std::invalid_argument&) {
            node->error(); //print error
        }
    } else if(child->grammarSign == "BROJ") {
        try {
            long long num = stoi(child->lexUnit); //we assume it fits into long long
            if(num < -2147483648 || num > 2147483647) { //value is not int
                node->error();
            } else {
                Attributes atr(FullType(Type::INT), false);
                if(atr.type.type == Type::INT)  //i think this should be okay but for testing
                    std::cout << "YES\n";
                return atr;
            }
        } catch(const std::exception&) {
            node->error(); //cannot be converted to int
        }
    } else if(child->grammarSign == "ZNAK") {
        try {
            IsCharCorrect(child->lexUnit);
            return Attributes(FullType(Type::CHAR), false);
        } catch(const std::exception&) {
            node->error();
        }
        
    } else if(child->grammarSign == "NIZ_ZNAKOVA") {
        try {
            isSeqCharCorrect(child->lexUnit);
            Attributes atr;
            atr.type.seq = true;
            atr.type.const_expr = true;
            atr.type.type = Type::CHAR;
            atr.l_expr = false;
            return atr;
        } catch(const std::exception&) {
            node->error();
        }
        
    } else if(child->grammarSign == "L_ZAGRADA") {
        return Expression(node->children[1]);
    }
    throw std::logic_error("Unknown first child in primary expression!");
}


bool checkImplicitCasting(std::vector<Parameter> types1, std::vector<Parameter> types2) {
    if(types1.size() != types2.size()) return false;
    int size = types1.size();
    for(int i = 0; i < size; i++) {
        if(!types1[i].type.isImplicitlyCastableToUnknownType(types2[i].type)) {
            return false;
        }
    }
    return true;
}

Attributes PostfixExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<primarni_izraz>") {
        return PrimaryExpression(firstChild);
    } else if(firstChild->grammarSign == "<postfiks_izraz>") {
        std::shared_ptr<Node> secondChild = node->children[1];
        if(secondChild->grammarSign == "L_UGL_ZAGRADA") {
            Attributes postfix_atr = PostfixExpression(node->children[0]);
            if(!postfix_atr.type.isSeqXType()) {
                node->error();
            }
            Attributes expression_atr = Expression(node->children[2]);
            if(!expression_atr.type.isImplicitlyCastableToInt()) {
                node->error();
            }
            FullType fullType;
            fullType.xType = true;
            Attributes ret_atr(fullType);
            ret_atr.l_expr = true; //? if X != const(T) WTF TODO TODO TODO TODO TODO TODO TODO
            return ret_atr;
        } else if(secondChild->grammarSign == "L_ZAGRADA") {
            std::shared_ptr<Node> thirdChild = node->children[2];
            if(thirdChild->grammarSign == "D_ZAGRADA") {
                Attributes postfix_atr = PostfixExpression(node->children[0]);
                if(!postfix_atr.isFunction || !postfix_atr.parameters.empty() || postfix_atr.return_type.type == Type::NONE) {
                    node->error();
                }                
                Attributes atr_ret;
                atr_ret.return_type = postfix_atr.return_type;
                atr_ret.l_expr = false;
                return atr_ret;
            } else if(thirdChild->grammarSign == "<lista_argumenata>") {
                Attributes postfix_atr = PostfixExpression(node->children[0]);
                Attributes list_arguments = ListArguments(node->children[2]);
                if(!postfix_atr.isFunction || postfix_atr.parameters.empty() || postfix_atr.return_type.type == Type::NONE || 
                    !checkImplicitCasting(list_arguments.parameters, postfix_atr.parameters)) {
                    node->error();
                }
                Attributes ret_atr;
                ret_atr.type = postfix_atr.return_type;
                ret_atr.l_expr = false;
                return ret_atr;
            }
        } else if(secondChild->grammarSign == "OP_INC" || secondChild->grammarSign == "OP_DEC") {
            Attributes postfix_atr = PostfixExpression(node->children[0]);
            if(postfix_atr.l_expr == false || !postfix_atr.type.isImplicitlyCastableToInt()) {
                node->error();
            } else {
                return Attributes(FullType(Type::INT), false);
            }
        }
    }
    throw std::logic_error("Error in postfix expression!");
}


Attributes ListArguments(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<izraz_pridruzivanja>") {
        return AssignmentExpression(firstChild);
    } else if(firstChild->grammarSign == "<lista_argumenata>") {
        Attributes list_atr = ListArguments(node->children[0]);
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        for(Parameter param: assign_atr.parameters) {
            list_atr.parameters.push_back(param);
        }
        return list_atr;
    }
    throw std::logic_error("Error in ListArguments!");
}

Attributes UnaryExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<postfiks_izraz>") {
        return PostfixExpression(firstChild);
    } else if(firstChild->grammarSign == "OP_INC" || firstChild->grammarSign == "OP_DEC") {
        Attributes unary_expr = UnaryExpression(node->children[1]);
        if(unary_expr.l_expr == false || !unary_expr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    } else if(firstChild->grammarSign == "<unarni_operator>") {
        Attributes cast_atr = CastExpression(node->children[1]);
        if(!cast_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in Unary Expression!");
}


Attributes CastExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<unarni_izraz>") {
        return UnaryExpression(firstChild);
    } else if(firstChild->grammarSign == "L_ZAGRADA") {
        Attributes nameType_expr = TypeName(node->children[1]);
        Attributes castExpr_atr = CastExpression(node->children[3]);
        if(!castExpr_atr.type.isExplicitlyCastable(nameType_expr.type)) {
            node->error();
        }
        return Attributes(nameType_expr.type, false);
    }
     throw std::logic_error("Error in postfix expression!");  
}


Attributes MultiplicativeExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<cast_izraz>") {
        return CastExpression(firstChild);
    } else if(firstChild->grammarSign == "<multiplikativni_izraz>") {
        Attributes mul_expr = MultiplicativeExpression(firstChild);
        if(!mul_expr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes cast_atr = CastExpression(node->children[2]);
        if(!cast_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
      throw std::logic_error("Error in Multiplicative expression!"); 
}


Attributes AditiveExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<multiplikativni_izraz>") {
        return MultiplicativeExpression(firstChild);
    } else if(firstChild->grammarSign == "<aditivni_izraz>") {
        Attributes adi_atr = AditiveExpression(firstChild);
        if(!adi_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes mul_atr = MultiplicativeExpression(node->children[2]);
        if(!mul_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes RelationalExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<aditivni_izraz>") {
        return AditiveExpression(firstChild);
    } else if(firstChild->grammarSign == "<odnosni_izraz>") {
        Attributes rel_atr = RelationExpression(firstChild);
        if(!rel_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes adi_atr = AditiveExpression(node->children[2]);
        if(!adi_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in relational expression!");
}

Attributes EqualityExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<odnosni_izraz>") {
        return RelationalExpression(firstChild);
    } else if(firstChild->grammarSign == "<jednakosni_izraz>") {
        Attributes equ_atr = EqualityExpression(firstChild);
        if(!equ_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes rel_atr = RelationalExpression(node->children[2]);
        if(!rel_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in equality expression!");
}

Attributes BitAndExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<jednakosni_izraz>") {
        return EqualityExpression(firstChild);
    } else if(firstChild->grammarSign == "<bin_i_izraz>") {
        Attributes bit_atr = BitAndExpression(firstChild);
        if(!bit_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes equ_atr = EqualityExpression(node->children[2]);
        if(!equ_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in bitand expression!");
}


Attributes BitXorExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_i_izraz>") {
        return BitAndExpression(firstChild);
    } else if(firstChild->grammarSign == "<bin_xili_izraz>") {
        Attributes xili_atr = BitXorExpression(firstChild);
        if(!xili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes bit_and_atr = BitAndExpression(node->children[2]);
        if(!bit_and_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in bitxor expression!");
}


Attributes BitOrExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_xili_izraz>") {
        return BitXorExpression(firstChild);
    } else if(firstChild->grammarSign == "<bin_ili_izraz>") {
        Attributes ili_atr = BitOrExpression(firstChild);
        if(!ili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes bit_xor_atr = BitXorExpression(node->children[2]);
        if(!bit_xor_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in bit or expression!");
}


Attributes LogAndExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_ili_izraz>") {
        return BitOrExpression(firstChild);
    } else if(firstChild->grammarSign == "<log_i_izraz>") {
        Attributes log_i_atr = LogAndExpression(firstChild);
        if(!log_i_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes bit_or_atr = BitOrExpression(node->children[2]);
        if(!bit_or_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in log and expression!");
}


Attributes LogOrExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<log_i_izraz>") {
        return LogAndExpression(firstChild);
    } else if(firstChild->grammarSign == "<log_ili_izraz>") {
        Attributes log_ili_atr = LogOrExpression(firstChild);
        if(!log_ili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        Attributes log_i_atr = LogAndExpression(node->children[2]);
        if(!log_i_atr.type.isImplicitlyCastableToInt()) {
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in log or expression!");
}

Attributes AssignmentExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<log_ili_izraz>") {
        return LogOrExpression(firstChild);
    } else if(firstChild->grammarSign == "<postfiks_izraz>") {
        Attributes post_atr = PostfixExpression(firstChild);
        if(post_atr.l_expr == false) {
            node->error();
        }
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        if(!assign_atr.type.isImplicitlyCastableToUnknownType(post_atr.type)) {
            node->error();
        }
        return Attributes(post_atr.type, 0);
    }
    throw std::logic_error("Error in assignment expression!");
}

Attributes Expression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<izraz_pridruzivanja>") {
        return AssignmentExpression(firstChild);
    } else if(firstChild->grammarSign == "<izraz>") {
        Attributes expr = Expression(node->children[0]);
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        return Attributes(assign_atr.type, 0);
    }
    throw std::logic_error("Error in aditive expression!");
}


//just for compiling
Attributes TypeName(std::shared_ptr<Node> node) {
  Attributes atr;
  return atr;
}
