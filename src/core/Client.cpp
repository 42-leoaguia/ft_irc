/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 18:18:23 by davmendo          #+#    #+#             */
/*   Updated: 2026/09/01 20:17:13 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// In-class initialiser above is only a declaration in C++98: this is the
// definition, needed when MAX_LINE is bound to a reference (std::min, ...).
const size_t	Client::MAX_LINE;

Client::Client(int fd, const std::string &host): _fd(fd), _host(host), _in()
{
}

// The socket belongs to Server: it is closed in Server::disconnect(), never
// here, so destroying a Client never touches the fd (no double close).
Client::~Client()
{
}

int	Client::getFd() const
{
	return (_fd);
}

const std::string	&Client::getHost() const
{
	return (_host);
}

// --- input buffer (issue #5)

void	Client::appendToInBuffer(const char *data, size_t len)
{
	if (data == 0 || len == 0)
		return ;
	_in.append(data, len);
}

// Pops the first complete line out of _in, without its terminator.
// Accepts "\r\n" and a bare "\n" (nc does not send '\r' by default).
// A line whose payload exceeds MAX_LINE - 2 is not IRC: it is discarded and
// the next one is examined, so one bad line never stalls the ones behind it.
// Returns false, leaving `out` untouched, when no full line is buffered yet.
bool	Client::extractLine(std::string &out)
{
	for (;;)
	{
		std::string::size_type	end = _in.find('\n');
		if (end == std::string::npos)
			return (false);
		std::string::size_type	cut = end;
		if (cut > 0 && _in[cut - 1] == '\r')
			--cut;
		if (cut <= MAX_LINE - 2)
		{
			out.assign(_in, 0, cut);
			_in.erase(0, end + 1);
			return (true);
		}
		_in.erase(0, end + 1);   // oversize: drop it and keep looking (417 hook)
	}
}

// True when the bytes after the last terminator (the line still being
// received) can no longer fit in a valid message: a peer that never sends
// "\n" must not be allowed to grow _in forever.
bool	Client::inputOverflow() const
{
	std::string::size_type	last = _in.rfind('\n');
	std::string::size_type	pending;

	if (last == std::string::npos)
		pending = _in.size();
	else
		pending = _in.size() - last - 1;
	return (pending > MAX_LINE - 2);
}
