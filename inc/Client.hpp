/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:23:40 by davmendo          #+#    #+#             */
/*   Updated: 2026/09/01 20:59:47 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_CLIENT_HPP
#define FT_IRC_CLIENT_HPP

#include <cstddef>
#include <string>

class Client
{
public:
	// RFC 2812: a message is at most 512 bytes, CRLF included, so the
	// payload handed to the parser is at most MAX_LINE - 2 bytes.
	static const size_t	MAX_LINE = 512;

	Client(int fd, const std::string &host);
	~Client();                            // does not close _fd: Server owns the socket

	int					getFd() const;
	const std::string	&getHost() const;

	// --- input buffer (issue #5)
	void	appendToInBuffer(const char *data, size_t len);
	bool	extractLine(std::string &out);   // out valid only when true; oversize lines dropped
	bool	inputOverflow() const;           // unterminated tail past MAX_LINE - 2 -> drop client

private:
	Client(const Client &);                // not copyable
	Client &operator=(const Client &);

	int			_fd;
	std::string	_host;
	std::string	_in;                       // bytes received, not yet framed into lines
};

#endif
