/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 12:40:39 by liafonse          #+#    #+#             */
/*   Updated: 2026/10/28 12:40:40 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PART_HPP
#define PART_HPP

#include "Command.hpp"

class Part : public Command
{
public:
	Part();
	virtual ~Part();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params);
};

#endif