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
    Server server_sock(av[1], av[2]);
    return EXIT_SUCCESS;
}
