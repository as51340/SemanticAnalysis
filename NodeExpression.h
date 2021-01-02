#pragma once
#include <vector>
#include <string>
#include <memory>

class NodeExpression {
public:
    
    std::string type;
    std::vector<std::shared_ptr<std::string>> types;
    bool l_express = false; //not assigned
    std::vector<std::shared_ptr<std::string>> names;  //not assigned TODO check what other types exist 
    
    bool isFunction = false; 
    
    
    NodeExpression();
    NodeExpression(std::string type, bool l_express);
    
    
    void setTypes(std::vector<std::string> types);

    void setNames(std::vector<std::string> names);
    
};