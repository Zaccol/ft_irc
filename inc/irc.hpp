#define ERR_NEEDMOREPARAMS	 	" :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED 	"462 :Unauthorized command (already registered)\r\n"
#define ERR_NONICKNAMEGIVEN 	"431 :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME 	" :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE 		" :Nickname is already in use\r\n"
#define ERR_NORECIPIENT 		"411 :No recipient given ("
#define ERR_NOTEXTTOSEND 		"412 :No text to send\r\n"
#define ERR_CANNOTSENDTOCHAN 	" :Cannot send to channel\r\n"
#define ERR_NOSUCHNICK 			" :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL		" :No such channel\r\n"
#define ERR_CHANOPRIVSNEEDED	" :You're not channel operator\r\n"
#define ERR_USERNOTINCHANNEL 	" :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL 		" :You're not on that channel\r\n"
#define ERR_USERONCHANNEL 		" :is already on channel\r\n"
#define ERR_PASSWDMISMATCH		"464 :Password incorrect\r\n"


#define RPL_WELCOME				" :Welcome to the Internet Relay Network "
#define RPL_YOURHOST(nick)		" 002 "+nick+ " :Your host is ft_irc_all, running version 42.1\r\n"
#define RPL_CREATED(nick)		" 003 "+nick+ " :This server was created today\r\n"
#define RPL_MYINFO(nick)		" 004 " +nick+" ft_irc_all version 42.1 without any modes\r\n"
#define RPL_INVITING			"<channel> <nick>\r\n"
// #define RPL_INVITING(channel,nick)	(channel + " " nick + "\r\n")
// #define RPL_INVITING(chan,nick)		std::cout << chan << " " << nick << "\r\n";
#define RPL_TOPIC				"<channel> :<topic>\r\n"
#define RPL_NOTOPIC				" :No topic is set\r\n"
#define RPL_NAMERPLY			"( "=" ) <channel> :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )\r\n"
#define RPL_ENDOFNAMES			" :End of NAMES list.\r\n"
#define RPL_ENDOFWHO			" :End of WHO list.\r\n"


#define HOST	"Albulilus"

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <utility>
#include <stdio.h>
#include <csignal>
#include <cctype>
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "commande.hpp"
#include "message.hpp"