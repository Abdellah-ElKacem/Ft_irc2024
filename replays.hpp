#pragma once

#define ERR_UNKNOWNCOMMAND(hostname, nick, arg) ":" + hostname + " 421 " + nick + " " + arg + " :Unknown command\r\n"

#define ERR_NEEDMOREPARAMS(nick, hostname, cmd) ":" + hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"

#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"

#define RPL_CHANNELMODEIS(hostname, nick, channel, modes) ":" + hostname + " 324 " + nick + " " + channel + " " + modes + "\r\n"

#define ERR_CHANOPRIVSNEEDED(hostname, nick, chann) ":" + hostname + " 482 " + nick + " " + chann + " :You're not channel operator\r\n"

#define ERR_USERNOTINCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 441 " + nick + " " + nick2 + " " + chann + " " + " :They aren't on that channel\r\n"

#define ERR_NOSUCHNICK(hostname, nick) ":" + hostname + " 401 " + nick + " "  + " :No such nick\r\n"

#define ERR_KEYSET(hostname, nick, chann) ":" + hostname + " 467 " + nick + " " + chann + " :Channel key already set\r\n"

#define ERR_UNKNOWNMODE(hostname, nick, c) ":" + hostname + " 472 " + nick + " " + c + " :is unknown mode char to me\r\n"

#define ERR_NOTREGISTERED(nick, hostname) ":" + hostname + " 451 " + nick + " :You have not registered\r\n"

#define ERR_USERONCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 443 " + nick + " " + nick2 + " " + chann + " :is already on channel\r\n"

#define RPL_INVITING(hostname, nick, invited, chann) ":" + hostname + " 341 " + nick + " " + invited + " " + chann + " :Inviting " + invited + " to " + chann + "\r\n"

#define ERR_NOTONCHANNEL(hostname, chann) ":" + hostname + " 442 " + chann + " " + ":You're not on that channel\r\n"

#define RPL_TOPIC(hostname, nick, chann, topic) ":" + hostname + " 332 " + nick + " " + chann + " :" + topic + "\r\n"

#define ERR_CANNOTSENDTOCHAN(hostname, nick, channel) ":" + hostname + " 404 " + nick + " " + channel + " :Cannot send to channel\r\n"

#define RPL_NOTOPIC(hostname, nick, chann) ":" + hostname + " 331 " + nick + " " + chann + " :No topic is set.\r\n"

#define RPL_WELCOME(nick, hostname) ":" + hostname + " 001 " + nick + " :Welcome " + nick + " to the ft_irc network !\r\n"

#define RPL_YOURHOST(nick, hostname) ":" + hostname + " 002 " + nick + " :Your host is " + hostname + " running version 1.0 !\r\n"

#define RPL_CREATED(nick, hostname) ":" + hostname + " 003 " + nick + " :This server was created 2023-9-15 !\r\n"

#define RPL_MYINFO(nick, hostname) ":" + hostname + " 004 " + nick + " :Host: " + hostname + ", Version: 1.0, User mode: none, Channel modes: o, t, k, i !\r\n"

#define ERR_PASSWDMISMATCH(nick, hostname) ":" + hostname + " 464 " + nick + " :Password incorrect !\r\n"

#define ERR_ALREADYREGISTERED(nick, hostname) ":" + hostname + " 462 " + nick + " :You may not reregister !\r\n"

#define ERR_ERRONEUSNICKNAME(nick, hostname) ":" + hostname + " 432 " + nick + " :Erroneus nickname !\r\n"

#define ERR_NONICKNAMEGIVEN(nick, hostname) ":" + hostname + " 431 " + nick + " :No nickname given !\r\n"

#define ERR_NICKNAMEINUSE(nick, hostname) ":" + hostname + " 433 " + nick + " :Nickname is already in use !\r\n"

#define RPL_NICKCHANGE(oldNick, nick, hostname) ":" + oldNick + " NICK " + nick + "\r\n"

#define ERR_ALREADYREGISTERED(nick, hostname) ":" + hostname + " 462 " + nick + " :You may not reregister !\r\n"

#define ERR_BADCHANNELNAME(nick, hostname, channelname) ":" + hostname + " 476 " + nick + " " + channelname + " :Invalid channel name." + "\r\n"

#define ERR_CHANNELISFULL(nick, channelName) ":" + nick + " 471 " + channelName + " :Cannot join channel (+l)\r\n"

#define ERR_BADCHANNELKEY(nick, hostname, channelName) ":" + hostname + " 475 " + nick + " " + channelName + " :Cannot join channel (+K) - bad key\r\n"

#define ERR_INVITEONLY(nick, channelName) ":" + nick + " 473 " + channelName + " :Cannot join channel (+i)\r\n"

#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"

#define RPL_TOPICDISPLAY(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + topic + "\r\n"

#define RPL_TOPICWHOTIME(topicsetter, time, nick, hostname, channelName) ":" + hostname + " 333 " + nick + " " + channelName + " " + topicsetter + "!~" + topicsetter + "@" + hostname + " " + time + "\r\n"

#define RPL_NAMREPLY(hostname, clients, channelname, nick) ":" + hostname + " 353 " + nick + " = " + channelname + " :" + clients + "\r\n"

#define RPL_ENDOFNAMES(hostname, nick, channelname) ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"

#define RPL_MODEIS(channel, hostname, mode) ":" + hostname + " MODE " + channel + " " + mode + "\r\n"

#define RPL_MODEISLIMIT(channel, hostname, mode, newlimit) ":" + hostname + " MODE " + channel + " " + mode + " " + newlimit + "\r\n"

#define RPL_MODEISOP(channel, hostname, mode, target) ":" + hostname + " MODE " + channel + " " + mode + " " + target + "\r\n"

#define ERR_INVALIDMODEPARAM(channel, hostname, flag) ":" + hostname + " 696 " + channel + " " + flag + " * you must specifiy a parameter for the op mode\r\n"

#define ERR_NOTOP(hostname, channel) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator\r\n"

#define RPL_CREATIONTIME(hostname, channelName, nick, time) ":" + hostname + " 329 " + nick + " " + channelName + " " + time + "\r\n"

#define RPL_CHANNELMODES(hostname, channelName, nick, channelmode) ":" + hostname + " 324 " + nick + " " + channelName + " " + channelmode + "\r\n"

#define RPL_INVITE(nick, username, clienthostname, invited, channel) ":" + nick + "!" + username + "@" + clienthostname + " INVITE " + invited + " :" + channel + "\r\n"

#define ERR_INPUTTOOLONG(nick, hostname) ":" + hostname + " 417 " + nick + " :Input line was too long !\r\n"

#define RPL_VIEWTOPIC(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + topic + "\r\n"

#define RPL_VIEWTIME(hostname, nick, channel, time) ":" + hostname + " 333 " + nick + " " + channel + " " + nick + " " + time + "\r\n"

#define RPL_SETTOPIC(nick, user, hostname, channel, topic) ":" + nick + "!" + user + "@" + hostname + " TOPIC " + channel + " :" + topic + "\r\n"
// #define RPL_SETTOPIC(nick, hostname, channel, topic) ":" + nick + "!" + nick + "@" + hostname + " TOPIC " + channel + " :" + topic + "\r\n"

#define RPL_BOT(hostname, nick, message) ":" + hostname + " 001 " + nick + " Dad joke: " + message + "\r\n"

#define ERR_BOT(hostname, nick) ":" + hostname + " 450 " + nick + "enter [BOT generate] to generate a joke" + "\r\n"

#define RPL_YOUREOPER(hostname, nick) ":" + hostname + " 381 " + nick + ":You are now an IRC operator\r\n"

#define RPL_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"

#define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define ERR_NO_NICK_CHNL(hostname, nick) ":" + hostname + " 401 " + nick + " "  + " :No such nick/channel\r\n"
#define ERR_NORECIPIENT(hostname, nick ,cmd)  ":" + hostname  + " 411 " + nick + " :No recipient given (" + cmd + ")\r\n"
#define ERR_NOTEXTTOSEND(hostname, nick)  ":" + hostname  + " 412 " + nick + " :No text to send\r\n"



#define RPL_ADDMODET(hostname, nick, channel, mode, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + "\r\n"
#define RPL_DELMODET(hostname, nick, channel, mode, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + "\r\n"

#define RPL_ADDMODE(hostname, nick, channel, mode, arg, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + " " + arg + "\r\n"
#define RPL_DELMODE(hostname, nick, channel, mode, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + " " + "\r\n"
