#include "Server.hpp"
#include "Clients.hpp"

int main(int ac, char *av[]) {

    if (ac != 3) {
        if (ac == 1) {
            std::cerr << "Error: need a port and a password\n";
            return EXIT_FAILURE;
        } else if (ac == 2) {
            std::cerr << "Error: need a password\n";
            return EXIT_FAILURE;
        } else {
            std::cerr << "Error: need only 2 arguments\n";
            return EXIT_FAILURE;
        }
    }
    std::string port = av[1], password = av[2];
    if (port.empty() || password.empty()) {
        if (port.empty() && password.empty())
            std::cerr << "Error: need a valid port and a valid password\n";
        else if (port.empty())
            std::cerr << "Error: need a valid port\n";
        else
            std::cerr << "Error: need a valid password\n";
        return EXIT_FAILURE;
    }
    Server server_sock(port, password);
    return EXIT_SUCCESS;
}
