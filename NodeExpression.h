#pragma once
#include <vector>
#include <string>
#include <memory>

class NodeExpression {
    
public:
    
    std::string type;
    std::vector<std::string> types; //string is copied when pushed to vector
    bool l_express = false; 
    std::vector<std::string> names;  
    
    bool isFunction = false; 
    
    
    NodeExpression();
    NodeExpression(std::string, bool);
    
    
    void setTypes(std::vector<std::string>&);

    void setNames(std::vector<std::string>&);
    
};