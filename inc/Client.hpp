/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:23:40 by davmendo          #+#    #+#             */
/*   Updated: 2026/09/17 14:49:15 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_CLIENT_HPP
#define FT_IRC_CLIENT_HPP

#include <cstddef>	// size_t
#include <string>	// std::string

/*
o server cria um client em acceptClient() para cada conexao aceita e o apaga
em disconnect(). 

o client só guarda dados: quem chama recv(), send() e close() é sempre o Server
o que ele guarda hoje:
- o fd e o IP da conexão;
- o buffer de entrada (_in), onde os bytes esperam até formar uma linha;
- o nickname, que o comando NICK vai preencher (issue #12).
*/
class Client
{
public:
	// RFC 2812: uma mensagem IRC tem no máximo 512 bytes, contando o "\r\n".
	// entao o texto de uma linha, sem o "\r\n", tem no maximo
	// MAX_LINE - 2 = 510 bytes
	static const size_t	MAX_LINE = 512;

	// fd: socket devolvido por accept(). host: IP do cliente em texto
	Client(int fd, const std::string &host);
	// nao fecha o _fd: o socket é do server, que o fecha em disconnect()
	~Client();

	// getters
	int					getFd() const;
	const std::string	&getHost() const;
	const std::string	&getNickname() const;   // usado pelo Channel


	// TODO issue #6: vira queue(). por enquanto não envia nada
	void	sendMessage(const std::string &msg);

	// guarda os len bytes que o recv() devolveu, mesmo que sejam meio comando
	void	appendToInBuffer(const char *data, size_t len);
	// tira do buffer a próxima linha completa, já sem o "\r\n". devolve false,
	// sem mexer em out, quando ainda não há linha completa.
	// linhas com mais de 510 bytes são descartadas
	bool	extractLine(std::string &out);
	// true quando o pedaço sem "\n" já passou de 510 bytes: ele nunca vai virar
	// uma linha válida, então o Server derruba o cliente
	bool	inputOverflow() const;

private:

	Client(const Client &);
	Client &operator=(const Client &);

	int			_fd;		// socket desta conexão
	std::string	_host;		// IP do cliente, ex.: "127.0.0.1"
	std::string	_nickname;	// preenchido pelo NICK (issue #12)
	std::string	_in;		// bytes que ainda não formaram uma linha completa
};

#endif
