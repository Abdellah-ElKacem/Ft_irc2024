#include "Server.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    struct hostent *info;
if (info == NULL) {
    std::cout << "asdas\n";
    return 1;
}

    info = gethostbyname("www.google.com");
    std::cout << info->h_addr_list[0] << std::endl;
}