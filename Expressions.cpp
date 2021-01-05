#include "Expressions.h"

#include "Functions.h"

// clang-format off
char IsCharCorrect(std::string lexUnit) {
    //std::cerr << "Lex unit: " << lexUnit << std::endl;
    if(lexUnit[0] == '\'') { //remove simple quotation marks '
        //std::cerr << "YES" << std::endl;
        lexUnit = lexUnit.substr(1, lexUnit.size() - 2);
    }
    //std::cerr << "Lex unit after substring: " << lexUnit << std::endl;
    if(lexUnit.size() == 1 && lexUnit[0] != '\\') {
        //std::cerr << "NO" << std::endl;
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
            //std::cerr << "LexUnit: " << child->lexUnit << " " << child->local_scope.size() << std::endl;
            /*std::shared_ptr<Node> complexCommand = child->getClosestScope();
            std::cerr << complexCommand->local_scope.size() << std::endl;
            if(complexCommand->local_scope.count(child->lexUnit) > 0 ){
                return complexCommand->local_scope[child->lexUnit];
            }
            std::cerr << "Cannot find variable in scope!" << std::endl;
            std::cerr << child->lexUnit << std::endl;
            std::cerr << complexCommand->local_scope[child->lexUnit].isFunction << std::endl;
            node->error(); //print error */
            Attributes atr = child->findScope(child->lexUnit);
            //std::cerr << child->lexUnit << " " << atr.fullType.type << std::endl;
            return atr;
            //exception and we need to print error
        } catch(const std::invalid_argument&) {
            std::cerr << "Logic error in scope!" << std::endl;
            node->error(); //print error
        }
    } else if(child->grammarSign == "BROJ") {
        try {
            long long num = stoi(child->lexUnit); //we assume it fits into long long
            if(num < -2147483648 || num > 2147483647) { //value is not int
                std::cerr << "Cannot convert to int" << std::endl;
                node->error();
            } else {
                Attributes atr(FullType(Type::INT), false);
                if(atr.fullType.type == Type::INT)  //i think this should be okay but for testing
                    //std::cout << "YES\n";
                return atr;
            }
        } catch(const std::exception&) {
            std::cerr << "Cannot convert to int" << std::endl;
            node->error(); //cannot be converted to int
        }
    } else if(child->grammarSign == "ZNAK") {
        try {
            IsCharCorrect(child->lexUnit);
            return Attributes(FullType(Type::CHAR), false);
        } catch(const std::exception&) {
            std::cerr << "Char is not correct" << std::endl;
            node->error();
        }
        
    } else if(child->grammarSign == "NIZ_ZNAKOVA") {
        try {
            std::string s = isSeqCharCorrect(child->lexUnit);
            Attributes atr;
            atr.fullType.seq = true;
            atr.fullType.const_expr = true;
            atr.fullType.type = Type::CHAR;
            atr.l_expr = false;
            //std::cerr << s << std::endl;
            //std::cerr << s.size() << std::endl;
            atr.elem_num = s.size();
            return atr;
        } catch(const std::exception&) {
            std::cerr << "Char sequence is not correct" << std::endl;
            node->error();
        }
        
    } else if(child->grammarSign == "L_ZAGRADA") {
        return Expression(node->children[1]);
    }
    throw std::logic_error("Unknown first child in primary expression!");
}


bool checkImplicitCasting(std::vector<Parameter> types1, std::vector<Parameter> types2) {
    //std::cerr << types1.size() << " " << types2.size() << std::endl;
    //std::cerr << types2[0].fullType.type << std::endl;
    if(types1.size() != types2.size()) return false;
    int size = types1.size();
    //std::cerr << "SIze: " << size << std::endl;
    for(int i = 0; i < size; i++) {
        if(!types1[i].fullType.isImplicitlyCastableToUnknownType(types2[i].fullType)) {
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
            //std::cerr << postfix_atr.fullType.isSeqXType() << std::endl;
            //std::cerr << postfix_atr.fullType.type << std::endl;
            //std::cerr << postfix_atr.fullType.seq << std::endl;
            if(!postfix_atr.fullType.isSeqXType()) {
                std::cerr << "Postfix error, line 132" << std::endl;
                node->error();
            }
            Attributes expression_atr = Expression(node->children[2]);
            if(!expression_atr.fullType.isImplicitlyCastableToInt()) {
                std::cerr << "Postfix error, LINE 137" << std::endl;
                node->error();
            }
            FullType fullType = postfix_atr.fullType;
            fullType.seq = false;
            fullType.xType = true;
            Attributes ret_atr(fullType);
            ret_atr.l_expr = true; //? if X != const(T) WTF TODO TODO TODO TODO TODO TODO TODO
            //tu ce vjv trebat postavit seq
            return ret_atr;
        } else if(secondChild->grammarSign == "L_ZAGRADA") {
            std::shared_ptr<Node> thirdChild = node->children[2];
            if(thirdChild->grammarSign == "D_ZAGRADA") {
                Attributes postfix_atr = PostfixExpression(node->children[0]);
                if(!postfix_atr.isFunction || !postfix_atr.parameters.empty() || postfix_atr.return_type.type == Type::NONE) {
                    std::cerr << "Postfix error, LINE 150" << std::endl;
                    node->error();
                } 
                //return postfix_atr;
                Attributes atr_ret;
                atr_ret.fullType = postfix_atr.return_type;
                atr_ret.l_expr = false;
                return atr_ret; 
            } else if(thirdChild->grammarSign == "<lista_argumenata>") {
                Attributes postfix_atr = PostfixExpression(node->children[0]);
                Attributes list_arguments = ListArguments(node->children[2]);
                //std::cerr << postfix_atr.isFunction << " " << postfix_atr.parameters.empty() << " " << postfix_atr.return_type.type << std::endl;
                if(!postfix_atr.isFunction || postfix_atr.parameters.empty() || postfix_atr.return_type.type == Type::NONE || 
                    !checkImplicitCasting(list_arguments.parameters, postfix_atr.parameters)) {
                        std::cerr << "Postfix error, LINE 162" << std::endl;
                    node->error();
                }
                //return postfix_atr;
                Attributes ret_atr;
                ret_atr.fullType = postfix_atr.return_type;
                ret_atr.l_expr = false;
                return ret_atr;
            }
        } else if(secondChild->grammarSign == "OP_INC" || secondChild->grammarSign == "OP_DEC") {
            Attributes postfix_atr = PostfixExpression(node->children[0]);
            if(postfix_atr.l_expr == false || !postfix_atr.fullType.isImplicitlyCastableToInt()) {
                std::cerr << "Postfix error, LINE 173" << std::endl;
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
        Attributes atr =  AssignmentExpression(firstChild);
        Parameter p;
        p.fullType = atr.fullType;
        atr.parameters.push_back(p);
        return atr;
    } else if(firstChild->grammarSign == "<lista_argumenata>") {
        Attributes list_atr = ListArguments(node->children[0]);
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        Parameter p;
        p.fullType = assign_atr.fullType;
        list_atr.parameters.push_back(p);
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
        //std::cerr << unary_expr.l_expr << std::endl;
        //std::cerr << unary_expr.fullType.isImplicitlyCastableToInt() << std::endl;
        if(unary_expr.l_expr == false || !unary_expr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Unarx error, LINE 206" << std::endl;
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    } else if(firstChild->grammarSign == "<unarni_operator>") {
        Attributes cast_atr = CastExpression(node->children[1]);
        if(!cast_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Unary error, LINE 213" << std::endl;
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
        if(!castExpr_atr.fullType.isExplicitlyCastable(nameType_expr.fullType)) {
            std::cerr << "Cast error, LINE 230" << std::endl;
            node->error();
        }
        return Attributes(nameType_expr.fullType, false);
    }
     throw std::logic_error("Error in postfix expression!");  
}


Attributes MultiplicativeExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<cast_izraz>") {
        return CastExpression(firstChild);
    } else if(firstChild->grammarSign == "<multiplikativni_izraz>") {
        Attributes mul_expr = MultiplicativeExpression(firstChild);
        if(!mul_expr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Multiplicative error, LINE 246" << std::endl;
            node->error();
        }
        Attributes cast_atr = CastExpression(node->children[2]);
        if(!cast_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Multiplicative error, LINE 251" << std::endl;
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
        //std::cerr << adi_atr.fullType.type << std::endl;
        if(!adi_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Aditive error, LINE 267" << std::endl;
            node->error();
        }
        Attributes mul_atr = MultiplicativeExpression(node->children[2]);
        //std::cerr << mul_atr.fullType.type << std::endl;
        if(!mul_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Aditive error, LINE 272" << std::endl;
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
        Attributes rel_atr = RelationalExpression(firstChild);
        if(!rel_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Relational error, LINE 288" << std::endl;
            node->error();
        }
        Attributes adi_atr = AditiveExpression(node->children[2]);
        if(!adi_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Relational error, LINE 292" << std::endl;
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
        if(!equ_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Equality error, LINE 308" << std::endl;
            node->error();
        }
        Attributes rel_atr = RelationalExpression(node->children[2]);
        if(!rel_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "Equality error, LINE 312" << std::endl;
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
        if(!bit_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitAnd error, LINE 327" << std::endl;
            node->error();
        }
        Attributes equ_atr = EqualityExpression(node->children[2]);
        if(!equ_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitAnd error, LINE 332" << std::endl;
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
        if(!xili_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitXor error, LINE 348" << std::endl;
            node->error();
        }
        Attributes bit_and_atr = BitAndExpression(node->children[2]);
        if(!bit_and_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitXor error, LINE 353" << std::endl;
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
        if(!ili_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitOr error, LINE 369" << std::endl;
            node->error();
        }
        Attributes bit_xor_atr = BitXorExpression(node->children[2]);
        if(!bit_xor_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "BitOr error, LINE 374" << std::endl;
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
        if(!log_i_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "LogAnd error, LINE 390" << std::endl;
            node->error();
        }
        Attributes bit_or_atr = BitOrExpression(node->children[2]);
        if(!bit_or_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "LogAnd error, LINE 395" << std::endl;
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
        if(!log_ili_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "LogOr error, LINE 411" << std::endl;
            node->error();
        }
        Attributes log_i_atr = LogAndExpression(node->children[2]);
        if(!log_i_atr.fullType.isImplicitlyCastableToInt()) {
            std::cerr << "LogOr error, LINE 416" << std::endl;
            node->error();
        }
        return Attributes(FullType(Type::INT), 0);
    }
    throw std::logic_error("Error in log or expression!");
}

Attributes AssignmentExpression(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> firstChild = node->children[0];
    if(firstChild->grammarSign == "<log_ili_izraz>") {
        //return LogOrExpression(firstChild);
        Attributes atr = LogOrExpression(firstChild);
        return atr;
    } else if(firstChild->grammarSign == "<postfiks_izraz>") {
        Attributes post_atr = PostfixExpression(firstChild);
        if(post_atr.l_expr == false) {
            std::cerr << "Assignment error, LINE 431" << std::endl;
            node->error();
        }
        Attributes assign_atr = AssignmentExpression(node->children[2]);
        //std::cerr << assign_atr.fullType.type << " " << post_atr.fullType.type << std::endl;
        if(!assign_atr.fullType.isImplicitlyCastableToUnknownType(post_atr.fullType)) {
            std::cerr << "Assignment error, LINE 436" << std::endl;
            node->error();
        }
        return Attributes(post_atr.fullType, 0);
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
        return Attributes(assign_atr.fullType, 0);
    }
    throw std::logic_error("Error in aditive expression!");
}
