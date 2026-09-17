/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 18:18:23 by davmendo          #+#    #+#             */
/*   Updated: 2026/09/17 14:49:57 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


const size_t	Client::MAX_LINE;

// começa com o buffer de entrada vazio e sem nickname
Client::Client(int fd, const std::string &host): _fd(fd), _host(host), _in()
{
}

// o socket pertence ao server: ele é fechado em Server::disconnect(), nunca
// aqui. assim, apagar um Client nunca mexe no fd e não há close duplo
Client::~Client()
{
}

// getters

int	Client::getFd() const
{
	return (_fd);
}

const std::string	&Client::getHost() const
{
	return (_host);
}

// vazio até o comando NICK (issue #12) preencher _nickname.
// o Channel usa para montar a lista de nomes do canal
const std::string	&Client::getNickname() const
{
	return (_nickname);
}

// provisorio: ainda nao existe buffer de saida (issue #6), então a mensagem é
// ignorada. Quando a #6 criar o queue(), o Channel passa a chamar o queue()
void	Client::sendMessage(const std::string &msg)
{
	(void)msg;
}

// o TCP entrega bytes, nao comandos. Um recv() pode trazer
// meio comando, um comando inteiro ou vários de uma vez. o Server guarda tudo
// aqui com appendToInBuffer() e so processa o que extractLine() devolve, que
// sao sempre linhas completas.

// acrescenta ao fim do buffer exatamente len bytes
// o recv() não coloca '\0' no fim
void	Client::appendToInBuffer(const char *data, size_t len)
{
	// nada a guardar. tambem evita append(NULL, len), que seria
	// comportamento indefinido
	if (data == 0 || len == 0)
		return ;
	_in.append(data, len);
}

// tira do inicio de _in a primeira linha completa, sem o terminador.
// aceita "\r\n", o padrao do IRC, e "\n" sozinho, porque o nc não manda '\r'.
// uma linha com mais de 510 bytes nao é IRC valido: ela é descartada e a
// próxima é examinada, para uma linha ruim não travar as que vem depois.
// devolve false, sem mexer em out, quando ainda não há linha completa.
bool	Client::extractLine(std::string &out)
{
	for (;;)
	{
		// 1. procura o fim da primeira linha. sem '\n'
		std::string::size_type	end = _in.find('\n');
		if (end == std::string::npos)
			return (false);
		// 2. cut = tamanho do texto. um '\r' antes do '\n' fica de fora
		std::string::size_type	cut = end;
		if (cut > 0 && _in[cut - 1] == '\r')
			--cut;
		// 3. dentro do limite: copia o texto e apaga a linha inteira do buffer
		if (cut <= MAX_LINE - 2)
		{
			out.assign(_in, 0, cut);
			_in.erase(0, end + 1);
			return (true);
		}
		// 4. linha grande demais: apaga e volta a procurar
		_in.erase(0, end + 1);
	}
}

// diz se o cliente passou do limite sem mandar '\n'. olha só o pedaço depois
// do último '\n', que é a linha que ainda está chegando. se esse pedaço ja
// passou de 510 bytes, nunca vai virar uma linha valida, e o Server derruba o
// cliente para o buffer não crescer sem limite
bool	Client::inputOverflow() const
{
	std::string::size_type	last = _in.rfind('\n');
	std::string::size_type	pending;

	// pending = bytes depois do último '\n', ou o buffer todo se nao ha '\n'
	if (last == std::string::npos)
		pending = _in.size();
	else
		pending = _in.size() - last - 1;
	// um '\r' no fim pode ser a primeira metade de um "\r\n" que ainda está a
	// caminho. ele nao é texto, entao nao conta: uma linha valida de 510 bytes
	// esperando o seu '\n' não é overflow
	if (pending > 0 && _in[_in.size() - 1] == '\r')
		--pending;
	return (pending > MAX_LINE - 2);
}
