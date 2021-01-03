#include "Expressions.h"


char IsCharCorrect(std::string lexUnit) {
    if(lexUnit.rfind("\'", 0) == 0) {
        lexUnit = lexUnit.substr(1);
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
    lexUnit = lexUnit.substr(1);
    std::string builder;
    int size = lexUnit.size();
    for(int i = 0; i < size; i++) {
        if(lexUnit[i] == '"') {
            throw std::invalid_argument("Sequence is correct error!");
        }
        if(lexUnit[i] == '\\') {
            if(i >= size -1) {
                throw std::invalid_argument("Sequence is correct error!");
            }
            try {
                char c = IsCharCorrect(lexUnit.substr(i, 2));
                //if c == null kako da to provjerin jbt  
                i++;    
                builder += c; 
            } catch(const std::exception&) {
                throw std::invalid_argument("Sequence is correct error!");
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
            Attributes atr = node->findScope(child->lexUnit); //if this command executes succesfully, variable exists in some scope, else we threw 
            //exception and we need to print error
            return atr;
        } catch(const std::invalid_argument&) {
            node->error();
        }
    } else if(child->grammarSign == "BROJ") {
        long long num = stoi(child->lexUnit); //we assume it fits into long long
        if(num < -2147483648 || num > 2147483647) { //value is not int
            node->error();
            exit(-1);
        } else {
            Attributes atr;
            atr.type = Type::INT;
            atr.l_expr = false;
            return atr;
        }
    } else if(child->grammarSign == "ZNAK") {
        try {
            IsCharCorrect(child->lexUnit);
            Attributes atr;
            atr.type = Type::CHAR;
            atr.l_expr = false;
            return atr;
        } catch(const std::exception&) {
            node->error();
            exit(-1);
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
            exit(-1);
        }
        
    } else if(child->grammarSign == "L_ZAGRADA") {
        Attributes expr_atr = Expression(node->children[1]);
        Attributes atr;
        atr.type = expr_atr.type;
        atr.l_expr = expr_atr.l_expr;
        return atr;
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
        Attributes primary_atr = PrimaryExpression(firstChild);
        Attributes atr;
        atr.type = primary_atr.type;
        atr.l_expr = primary_atr.l_expr;
        return atr;
    } else if(firstChild->grammarSign == "<postfix_izraz>") {
        std::shared_ptr<Node> secondChild = node->children[1];
        if(secondChild->grammarSign == "L_UGL_ZAGRADA") {
            Attributes postfix_atr = PostfixExpression(firstChild);
            if(!postfix_atr.type.isSeqXType()) {
                node->error();
                exit(-1);
            }
            Attributes expression_atr = Expression(node->children[2]);
            if(!expression_atr.type.isImplicitlyCastableToInt()) {
                node->error();
                exit(-1);
            }
            FullType fullType;
            fullType.xType = true;
            Attributes ret_atr(fullType);
            expression_atr.l_expr = true; //? if X != const(T) WTF
            return ret_atr;
        } else if(secondChild->grammarSign == "L_ZAGRADA") {
            std::shared_ptr<Node> thirdChild = node->children[2];
            if(thirdChild->grammarSign == "D_ZAGRADA") {
                Attributes postfix_atr = PostfixExpression(firstChild);
                if(!postfix_atr.isFunction) {
                    node->error();
                    exit(-1);
                }
                if(!postfix_atr.parameters.empty()) {
                    node->error();
                    exit(-1);
                }
                if(postfix_atr.return_type.type == Type::NONE) {
                    node->error();
                    exit(-1);
                }
                Attributes atr_ret;
                atr_ret.return_type = postfix_atr.return_type;
                atr_ret.l_expr = 0;
            } else if(thirdChild->grammarSign == "<lista_argumenata>") {
                Attributes postfix_atr = PostfixExpression(firstChild);
                Attributes list_arguments = ListArguments(thirdChild);
                if(!postfix_atr.isFunction) {
                    node->error();
                    exit(-1);
                }
                if(postfix_atr.parameters.empty()) {
                    node->error();
                    exit(-1);
                }
                if(postfix_atr.return_type.type == Type::NONE) {
                    node->error();
                    exit(-1);
                }
                Attributes ret_atr;
                ret_atr.type = postfix_atr.return_type;
                ret_atr.l_expr = 0;
                return ret_atr;
            }
        } else if(secondChild->grammarSign == "OP_INC" || secondChild->grammarSign == "OP_DEC") {
            Attributes postfix_atr = PostfixExpression(firstChild);
            if(postfix_atr.l_expr != 1 || !postfix_atr.type.isImplicitlyCastableToInt()) {
                node->error();
                exit(-1);
            } else {
                FullType fullType(Type::INT);
                Attributes atr(fullType, 0);
                return atr;
            }
        }
    }
    throw std::logic_error("Error in postfix expression!");
}


Attributes ListArguments(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<izraz_pridruzivanja>") {
        Attributes atr = AssignmentExpression(firstChild);
        Attributes atr_ret;
        atr_ret.parameters = atr.parameters;
        return atr_ret;
    } else if(firstChild->grammarSign == "<lista_argumenata>") {
        Attributes list_atr = ListArguments(firstChild);
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        Attributes atr;
        atr.parameters = list_atr.parameters;
        for(Parameter param: assign_atr.parameters) {
            atr.parameters.push_back(param);
        }
        return atr;
    }
    throw std::logic_error("Error in ListArguments!");
}

Attributes UnaryExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<postfix_izraz>") {
        Attributes post_atr = PostfixExpression(firstChild);
        Attributes ret_atr;
        ret_atr.type = post_atr.type;
        ret_atr.l_expr = post_atr.l_expr;
        return ret_atr;
    } else if(firstChild->grammarSign == "OP_INC" || firstChild->grammarSign == "OP_DEC") {
        Attributes unary_expr = UnaryExpression(node->children[1]);
        if(unary_expr.l_expr == false || !unary_expr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes atr;
        FullType fullType(Type::INT);
        atr.type = fullType;
        atr.l_expr = false;
        return atr;
    } else if(firstChild->grammarSign == "<unarni_operator>") {
        Attributes cast_atr = CastExpression(node->children[1]);
        if(!cast_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes atr;
        FullType fullType(Type::INT);
        atr.type = fullType;
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in Unary Expression!");
}


Attributes CastExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<unarni_izraz>") {
        Attributes unary_expr = UnaryExpression(firstChild);
        Attributes ret;
        ret.type = unary_expr.type;
        ret.l_expr = unary_expr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "L_ZAGRADA") {
        Attributes nameType_expr = NameType(node->children[1]);
        Attributes castExpr_atr = CastExpression(node->children[3]);
        if(!castExpr_atr.type.isExplicitlyCastable(nameType_expr.type)) {
            node->error();
            exit(-1);
        }
    }
     throw std::logic_error("Error in postfix expression!");  
}


Attributes MultiplicativeExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<cast_izraz>") {
        Attributes cast_atr = CastExpression(firstChild);
        Attributes atr;
        atr.type = cast_atr.type;
        atr.l_expr = cast_atr.l_expr;
        return atr;
    } else if(firstChild->grammarSign == "<multiplikativni_izraz>") {
        Attributes mul_expr = MultiplicativeExpression(firstChild);
        if(!mul_expr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes cast_atr = CastExpression(node->children[2]);
        if(!cast_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
      throw std::logic_error("Error in Multiplicative expression!"); 
}


Attributes AditiveExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<multiplikativni_izraz>") {
        Attributes mul_atr = MultiplicativeExpression(firstChild);
        Attributes atr;
        atr.type = mul_atr.type;
        atr.l_expr = mul_atr.l_expr;
        return atr;
    } else if(firstChild->grammarSign == "<aditivni_izraz>") {
        Attributes adi_atr = AditiveExpression(firstChild);
        if(!adi_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes mul_atr = MultiplicativeExpression(node->children[2]);
        if(!mul_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes RelationalExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<aditivni_izraz>") {
        Attributes rel_atr = AditiveExpression(firstChild);
        Attributes ret;
        ret.type = rel_atr.type;
        ret.l_expr = rel_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<odnosni_izraz>") {
        Attributes rel_atr = RelationExpression(firstChild);
        if(!rel_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes adi_atr = AditiveExpression(node->children[2]);
        if(!adi_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes EqualityExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<odnosni_izraz>") {
        Attributes rel_atr = RelationalExpression(firstChild);
        Attributes ret;
        ret.type = rel_atr.type;
        ret.l_expr = rel_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<jednakosni_izraz>") {
        Attributes equ_atr = EqualityExpression(firstChild);
        if(!equ_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes rel_atr = RelationalExpression(node->children[2]);
        if(!rel_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes BitAndExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<jednakosni_izraz>") {
        Attributes equ_atr = EqualityExpression(firstChild);
        Attributes ret;
        ret.type = equ_atr.type;
        ret.l_expr = equ_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<bin_i_izraz>") {
        Attributes bit_atr = BitAndExpression(firstChild);
        if(!bit_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes equ_atr = EqualityExpression(node->children[2]);
        if(!equ_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}


Attributes BitXorExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_i_izraz>") {
        Attributes bit_and_atr = BitAndExpression(firstChild);
        Attributes ret;
        ret.type = bit_and_atr.type;
        ret.l_expr = bit_and_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<bin_xili_izraz>") {
        Attributes xili_atr = BitXorExpression(firstChild);
        if(!xili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes bit_and_atr = BitAndExpression(node->children[2]);
        if(!bit_and_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}


Attributes BitOrExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_xili_izraz>") {
        Attributes bit_xor_atr = BitXorExpression(firstChild);
        Attributes ret;
        ret.type = bit_xor_atr.type;
        ret.l_expr = bit_xor_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<bin_ili_izraz>") {
        Attributes ili_atr = BitOrExpression(firstChild);
        if(!ili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes bit_xor_atr = BitXorExpression(node->children[2]);
        if(!bit_xor_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}


Attributes LogAndExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<bin_ili_izraz>") {
        Attributes bit_ili_atr = BitOrExpression(firstChild);
        Attributes ret;
        ret.type = bit_ili_atr.type;
        ret.l_expr = bit_ili_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<log_i_izraz>") {
        Attributes log_i_atr = LogAndExpression(firstChild);
        if(!log_i_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes bit_or_atr = BitOrExpression(node->children[2]);
        if(!bit_or_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}


Attributes LogOrExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<log_i_izraz>") {
        Attributes log_i_atr = LogAndExpression(firstChild);
        Attributes ret;
        ret.type = log_i_atr.type;
        ret.l_expr = log_i_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<log_ili_izraz>") {
        Attributes log_ili_atr = LogOrExpression(firstChild);
        if(!log_ili_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        Attributes log_i_atr = LogAndExpression(node->children[2]);
        if(!log_i_atr.type.isImplicitlyCastableToInt()) {
            node->error();
            exit(-1);
        }
        FullType fullType(Type::INT);
        Attributes atr(fullType);
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes AssignmentExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<log_ili_izraz>") {
        Attributes log_ili_atr = LogOrExpression(firstChild);
        Attributes ret;
        ret.type = log_ili_atr.type;
        ret.l_expr = log_ili_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "<postfix_izraz>") {
        Attributes post_atr = PostfixExpression(firstChild);
        if(post_atr.l_expr != 1) {
            node->error();
            exit(-1);
        }
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        if(!assign_atr.type.isExplicitlyCastable(post_atr.type)) {
            node->error();
            exit(-1);
        }
        Attributes atr;
        atr.type = post_atr.type;
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}

Attributes Expression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<izraz_pridruzivanja>") {
        Attributes assign_atr = AssignmentExpression(firstChild);
        Attributes ret;
        ret.type = assign_atr.type;
        ret.l_expr = assign_atr.l_expr;
        return ret;
    } else if(firstChild->grammarSign == "zraz>") {
        Attributes expr = Expression(firstChild);
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        Attributes atr;
        atr.type = assign_atr.type;
        atr.l_expr = false;
        return atr;
    }
    throw std::logic_error("Error in aditive expression!");
}
