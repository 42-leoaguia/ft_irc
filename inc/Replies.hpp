/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 17:10:34 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/02 17:10:47 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

/* 001 - 004 : Welcome */

#define RPL_WELCOME                 "001"
#define RPL_WELCOME_TEXT            "Welcome to the IRC server"

#define RPL_YOURHOST                "002"
#define RPL_YOURHOST_TEXT           "Your host is ircserv"

#define RPL_CREATED                 "003"
#define RPL_CREATED_TEXT            "This server was created"

#define RPL_MYINFO                  "004"
#define RPL_MYINFO_TEXT             "ircserv"


/* 331 - 366 : Channel */

#define RPL_NOTOPIC                 "331"
#define RPL_NOTOPIC_TEXT            "No topic is set"

#define RPL_TOPIC                   "332"

#define RPL_INVITING                "341"

#define RPL_NAMREPLY                "353"

#define RPL_ENDOFNAMES              "366"
#define RPL_ENDOFNAMES_TEXT         "End of /NAMES list"


/* 401 - 412 : Errors */

#define ERR_NOSUCHNICK              "401"
#define ERR_NOSUCHNICK_TEXT         "No such nick/channel"

#define ERR_NOSUCHCHANNEL           "403"
#define ERR_NOSUCHCHANNEL_TEXT      "No such channel"

#define ERR_CANNOTSENDTOCHAN        "404"
#define ERR_CANNOTSENDTOCHAN_TEXT   "Cannot send to channel"

#define ERR_NOORIGIN                "409"
#define ERR_NOORIGIN_TEXT           "No origin specified"

#define ERR_NORECIPIENT             "411"
#define ERR_NORECIPIENT_TEXT        "No recipient"

#define ERR_NOTEXT                  "412"
#define ERR_NOTEXT_TEXT             "No text to send"


/* 421 - 451 : Errors */

#define ERR_UNKNOWNCOMMAND          "421"
#define ERR_UNKNOWNCOMMAND_TEXT     "Unknown command"

#define ERR_NONICKNAMEGIVEN         "431"
#define ERR_NONICKNAMEGIVEN_TEXT    "No nickname given"

#define ERR_ERRONEUSNICKNAME        "432"
#define ERR_ERRONEUSNICKNAME_TEXT   "Erroneous nickname"

#define ERR_NICKNAMEINUSE           "433"
#define ERR_NICKNAMEINUSE_TEXT      "Nickname is already in use"

#define ERR_USERNOTINCHANNEL        "441"
#define ERR_USERNOTINCHANNEL_TEXT   "User is not in channel"

#define ERR_NOTONCHANNEL            "442"
#define ERR_NOTONCHANNEL_TEXT       "You're not on that channel"

#define ERR_USERONCHANNEL           "443"
#define ERR_USERONCHANNEL_TEXT      "User is already on channel"

#define ERR_NOTREGISTERED           "451"
#define ERR_NOTREGISTERED_TEXT      "You have not registered"


/* 461 - 464 : Errors */

#define ERR_NEEDMOREPARAMS          "461"
#define ERR_NEEDMOREPARAMS_TEXT     "Not enough parameters"

#define ERR_ALREADYREGISTERED       "462"
#define ERR_ALREADYREGISTERED_TEXT  "You may not reregister"

#define ERR_PASSWDMISMATCH          "464"
#define ERR_PASSWDMISMATCH_TEXT     "Password incorrect"


/* 471 - 482 : Channel errors */

#define ERR_CHANNELISFULL           "471"
#define ERR_CHANNELISFULL_TEXT      "Cannot join channel (+l)"

#define ERR_UNKNOWNMODE             "472"
#define ERR_UNKNOWNMODE_TEXT        "is unknown mode char to me"

#define ERR_INVITEONLYCHAN          "473"
#define ERR_INVITEONLYCHAN_TEXT     "Cannot join channel (+i)"

#define ERR_BADCHANNELKEY           "475"
#define ERR_BADCHANNELKEY_TEXT      "Cannot join channel (+k)"

#define ERR_BADCHANMASK             "476"
#define ERR_BADCHANMASK_TEXT        "Bad Channel Mask"

#define ERR_CHANOPRIVSNEEDED       "482"
#define ERR_CHANOPRIVSNEEDED_TEXT  "You're not channel operator"

#endif