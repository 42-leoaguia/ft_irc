/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 12:40:42 by liafonse          #+#    #+#             */
/*   Updated: 2026/10/28 12:40:43 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "Command.hpp"

class Topic : public Command
{
public:
	Topic();
	virtual ~Topic();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params);
};

#endif