#include "../Server.hpp"
#include "../channel.hpp"
#include <fstream>

void send_rep_(int fd, std::string msg)
{
    if (send(fd, msg.c_str() , msg.size(), 0) == -1)
        std::cerr << "ERROR: send\n";
}

int main(int ac, char *av[]) {
    // std::cout << ac << std::endl;
    if (ac != 6) {
        std::cout << "U must folow this steps : address -> port -> nickname -> cookies -> password .\nTry Again.. :/" << std::endl;
        return EXIT_FAILURE;
    }
    int fd_bot = 0;
    std::string pass = av[5];
    std::string nick = av[3];
    // std::cout << pass
    struct sockaddr_in __bot;
    fd_bot = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_bot < 0) {
        std::perror("socket");
        exit(EXIT_FAILURE);
    }
    __bot.sin_family = AF_INET;
    __bot.sin_port = htons(std::atoi(av[2]));
    __bot.sin_addr.s_addr = inet_addr(av[1]);
	if (connect(fd_bot, (struct sockaddr*)&__bot, sizeof(__bot)) != 0) {
        std::perror("connect");
        return EXIT_FAILURE;
    }
    sleep(3);
    char buffer[512];
    std::string str;
    str = "pass " + pass + "\r\n";
    write(fd_bot, str.c_str(), str.length());
    str = "user bot bot bot bot\r\n";
    write(fd_bot, str.c_str(), str.length());
    str = "nick " + nick + "\r\n";
    write(fd_bot, str.c_str(), str.length());
    ssize_t readd = read(fd_bot, buffer, 511);
    if (readd <= 0)
        return EXIT_FAILURE;
    buffer[readd] = '\0';
    std::string check, rr;
    check = buffer;
    rr = check.substr(check.find(" ") + 1, 3);
    if (rr == "464" || rr == "461") {
        std::cout << "U must put the correct Password ,Try Again.. :/" << std::endl;
        close(fd_bot);
        return EXIT_FAILURE;
    }
    if (rr == "433" || rr == "432") {
        std::cout << "This nickname is used or Error in NickName, Try Again.. :/" << std::endl;
        close(fd_bot);
        return EXIT_FAILURE;
    }
    while (1) {
        std::memset(buffer, 0, 512);
        ssize_t readd = read(fd_bot, buffer, 511);
        if (readd <= 0)
            break;
        buffer[readd] = '\0';
        str = buffer;
        std::cout << str << "->\n";
        if (str.empty())
            continue;
        if (str.substr(0, 4) == "PING") {
            std::string num_pong = "PONG " + str.substr(6);
            std::cout << num_pong << std::endl;
            send(fd_bot, num_pong.c_str(),num_pong.length(),0);
        }
        std::string test;
        test = str.substr(str.find(" ") + 1, 7);
        if (test == "PRIVMSG") {
            std::string nick = str.substr(1, str.find("!") - 1), cmd;
            size_t indx = str.find(" "), i = 0;
            while(indx != str.npos) {
                i++;
                if(i == 3) {
                    cmd = str.substr(indx + 1);
                    if (cmd.at(0) == ':')
                        cmd = cmd.substr(1);
                    if (cmd.back() == '\n')
                        cmd.pop_back();
                    if (cmd.back() == '\r')
                        cmd.pop_back();
                    break;
                }
                indx = str.find(" ", indx + 1);
            } if (cmd == "joke") {
                std::string line;
                int rand_val = std::rand() % 20 + 1, i = 0;
                std::ifstream f_joke("Jokes.txt");
                if(f_joke.fail()) {
                    std::cout<< "?!!!...\n";
                    continue;
                } while(getline(f_joke, line)) {
                    if(i == rand_val) {
                        std::string str1 = "PRIVMSG " + nick + " :" + line + " \r\n";
                        send(fd_bot, str1.c_str(),str1.length(),0);
                        break;
                    }
                    i++;
                }
            }
            else if (cmd.substr(0, 6) == "search") {
                std::string login = cmd.substr(7);
                std::string str1 = "PRIVMSG " + nick + " : the name is " + login + "\r\n";
                send(fd_bot, str1.c_str(),str1.length(),0);
            }
            else {
                std::string str1 = "PRIVMSG " + nick + " : Just \" joke \" and \" search with user \" ,Try Again..\r\n";
                send(fd_bot, str1.c_str(),str1.length(),0);
            }
            nick.clear();
        }
    }
    close(fd_bot);
    return EXIT_SUCCESS;
}