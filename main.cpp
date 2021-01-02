#include <iostream>
#include <sstream>
#include <vector>

//TODO
void parseInput() {
    std::string line;
    std::stringstream line_stream(line);
    while(std::getline(std::cin, line)) {
        line_stream.str(line);
        std::string item;
        std::vector<std::string> elements;
        while(line_stream >> item) {
            elements.push_back(item);
        }
        if(elements.size() == 1) {
            if(elements[0].compare("$") == 0) {
                std::cerr << "Imamo samo epsilon znak\n";
                //napravi nov
            }
        }
    }
}

int main(void) {
    
    
    return 0;
}