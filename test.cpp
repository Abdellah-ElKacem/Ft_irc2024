#include <sstream>
#include <iostream>
#include <string>

int main() {

    std::string s = "                            ssss s           fdfd        ddd";

    std::stringstream ss(s);

    std::string token;
    while (ss >> token)
    {
        std::cout << token << " ";
    }
    
}