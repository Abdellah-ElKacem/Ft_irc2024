#include "../Server.hpp"
#include "../channel.hpp"

int registr_bot(int fd_bot, std::string pass, std::string nick) {
    char buffer[512];
    std::string str;
    str = "pass " + pass + "\r\n";
    write(fd_bot, str.c_str(), str.length());
    str = "user bot bot bot bot\r\n";
    write(fd_bot, str.c_str(), str.length());
    str = "nick " + nick + "\r\n";
    write(fd_bot, str.c_str(), str.length());
    return 0;
}

void ft_send_msg(std::string msg , std::string nick, int fd)
{
    std::string form = "PRIVMSG " + nick + " : ";
    form += msg;
    form += "\r\n";
    send(fd, form.c_str(),form.length(),0);
}

void trim(std::string& str)
{
	size_t bigen = str.find_first_not_of(" \"{[");
	size_t last = str.find_last_not_of(" \"}]");
	if (last == std::string::npos)
		last = str.length() -1;
	if (bigen == std::string::npos)
		bigen = 0;
	str =  str.substr(bigen , (last - bigen + 1));
}
 
void fill_map(std::string& line, std::map<std::string, std::string>& map)
{
    std::string key, value;
    std::size_t pos = line.find(':');
	key = line.substr(0, pos - 1);
    value = line.substr(pos + 1);
	trim(value);
	trim(key);
    map[key] = value;
}
void split(std::string& text, std::map<std::string , std::string>& map)
{
    int i = 0;
    std::string line;
    while (i < text.size())
    {
        if (text[i] == ',')
        {
            fill_map(line, map);
            line.clear();
        }
        else
            line.push_back(text[i]);
        i++;
    }
    if (!line.empty())
        fill_map(line, map);
}
void ft_get_data(std::map<std::string, std::string>& map, std::string key, int fd, std::string& nick)
{
    // std::cout << "key ?:" << key << std::endl;
    // std::cout << "value :" << map[key] << std::endl;
    if (map[key] == "null")
        map[key] = "offline";
	std::string msg = key + ": " + map[key];
    ft_send_msg(msg, nick, fd);
}
void ft_get_lvl(std::map<std::string, std::string>& map, std::string key, int fd, std::string&nick)
{
	std::string str;
	std::string msg;
    if (key == "42 events")
        return ;
	str = map[key];
    // std::cout << "key : " << key << " "<< str << std::endl;
	std::size_t pos = str.find(':');
	std::size_t pos1 = str.find(':', pos +1);
	if (pos1 == std::string::npos)
		pos1 = pos;
    msg = key + " level: " + str.substr(pos1 + 1);
    ft_send_msg(msg, nick, fd);
}

void    ft_send_info(std::string& username, std::string& cookie, int fd, std::string& nick)
{
	int ok = 0;
	std::map<std::string , std::string> result;
    std::vector<std::string> lines;
	std::string url = "https://profile.intra.42.fr/users/" + username;
    std::string cmd = "curl " + url + " -s -b _intra_42_session_production=" + cookie + " > result.txt";
    std::cout << cmd << std::endl;
	for (size_t i = 0; i < username.length(); i++)
	{
		if (!std::isalpha(username[i]) && username[i] != '-')
		{
			ft_send_msg("format error .", nick, fd);
            return ;
		}
	}
    system(cmd.c_str());
    std::ifstream inputFile("result.txt");
    std::getline(inputFile, cmd);
	if (cmd == "{}")
	{
		ft_send_msg("User not found", nick , fd);
		return;
	}
	else if (!cmd.find("<html>"))
	{
		ft_send_msg("incorect cookie", nick , fd);
		ok = 1;
	}
	if (!ok)
	{
		split(cmd, result);
		ft_get_data(result, "full_name", fd, nick);
		ft_get_data(result, "location", fd, nick);
		ft_get_lvl(result, "cursus", fd, nick);
		ft_get_lvl(result, "42 events", fd, nick);
		ft_get_lvl(result, "C Piscine", fd, nick);
	}
    
	inputFile.close();
}


int is_privmsg(int fd_bot, std::string str, std::string nick, bool &ok,std::string &cookie) {
        std::string cmd;
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
            // exit(0);
        } if (cmd == "joke") {
            std::string line;
            int rand_val = std::rand() % 20 + 1, i = 0;
            std::ifstream f_joke("Jokes.txt");
            if(f_joke.fail()) {
                return 1;
            } while (getline(f_joke, line)) {
                if(i == rand_val) {
                    std::string str1 = "PRIVMSG " + nick + " :" + line + " \r\n";
                    send(fd_bot, str1.c_str(),str1.length(),0);
                    f_joke.close();
                    break;
                }
                i++;
            }
        }
        else if (cmd.substr(0, 6) == "search") {
            std::string login;
           if (cmd.length() <= 6)
           {
                ft_send_msg("please provide login\r\n", nick , fd_bot);
                return 0;
           }
            else
                login = cmd.substr(7);
            ft_send_info(login, cookie, fd_bot, nick);
        }
        else {
            std::string str1 = "PRIVMSG " + nick + " : Just \" joke \" and \" search with user \" ,Try Again..\r\n";
            send(fd_bot, str1.c_str(),str1.length(),0);
        }
        return 0;
}

int main(int ac, char *av[]) {
    bool ok = false;
    if (ac != 6) {
        std::cout << "U must folow this steps : address -> port -> nickname -> cookies -> password .\nTry Again.. :/" << std::endl;
        return EXIT_FAILURE;
    }
    int fd_bot = 0;
    std::string pass = av[5];
    std::string nick = av[3];
    std::string cookie(av[4]);
    struct sockaddr_in __bot;
    std::cout << "The Bot will be Installed on the Server with Nick_Name : \" " << nick << " \", Enjoy ;)" << std::endl;
    fd_bot = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_bot < 0) {
        std::perror("socket");
        exit(EXIT_FAILURE);
    }
    std::memset(&__bot, 0, sizeof(__bot));
    __bot.sin_family = AF_INET;
    __bot.sin_port = htons(std::atoi(av[2]));
    __bot.sin_addr.s_addr = inet_addr(av[1]);
	if (connect(fd_bot, (struct sockaddr*)&__bot, sizeof(__bot)) != 0) {
        close(fd_bot);
        std::perror("connect");
        return EXIT_FAILURE;
    }
    if (registr_bot(fd_bot, pass, nick)) {
        close(fd_bot);
        return EXIT_FAILURE;
    }
    char buffer[512];
    std::string str, rr;
    while (1) {
        std::memset(buffer, 0, 512);
        ssize_t readd = read(fd_bot, buffer, 511);
        if (readd <= 0)
            break;
        buffer[readd] = '\0';
        str = buffer;
        if (str.empty())
            continue;
        rr = str.substr(str.find(" ") + 1, 3);
        if (rr == "464" || rr == "461") {
            std::cout << "U must put the correct Password ,Try Again.. :/" << std::endl;
            close(fd_bot);
            return 1;
        }
        if (rr == "433" || rr == "432" || rr == "431") {
            std::cout << "This nickname is used or Error in NickName, Try Again.. :/" << std::endl;
            close(fd_bot);
            return 1;
        }
        if (str.substr(0, 4) == "PING") {
            std::string num_pong = "PONG " + str.substr(6);
            send(fd_bot, num_pong.c_str(),num_pong.length(),0);
        }
        std::string test;
        test = str.substr(str.find(" ") + 1, 7);
        if (test == "PRIVMSG")
        {
            std::string nick = str.substr(1, str.find("!") - 1);
            if(is_privmsg(fd_bot, str, nick, ok, cookie)) {
                std::string str1 = "PRIVMSG " + nick + " :no jokes in database, Try search command..\r\n";
                send(fd_bot, str1.c_str(),str1.length(),0);
                continue;
            }
            nick.clear();
        }
    }
    close(fd_bot);
    return EXIT_SUCCESS;
}