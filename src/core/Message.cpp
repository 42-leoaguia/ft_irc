/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:02:44 by leoaguia          #+#    #+#             */
/*   Updated: 2026/10/06 12:56:21 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

#include <cctype>	// std::isspace, std::toupper

const size_t	Message::MAX_PARAMS;

// espaco em branco separa as palavras da linha. o RFC so fala do espaco, mas
// um tab tambem conta: assim "  \t  " e uma linha vazia, como pede a issue #8.
// o cast evita comportamento indefinido com bytes acima de 127
static bool	isBlank(char c)
{
	return (std::isspace(static_cast<unsigned char>(c)) != 0);
}

// avanca i ate a proxima palavra. varios espacos seguidos valem por um
static void	skipBlanks(const std::string &line, std::string::size_type &i)
{
	while (i < line.size() && isBlank(line[i]))
		++i;
}

// devolve a palavra que comeca em i e deixa i no espaco logo depois dela
static std::string	nextWord(const std::string &line, std::string::size_type &i)
{
	std::string::size_type	start = i;

	while (i < line.size() && !isBlank(line[i]))
		++i;
	return (line.substr(start, i - start));
}

// separa a linha da esquerda para a direita, num unico passe:
// prefixo, comando e parametros
Message::Message(const std::string &line)
{
	std::string::size_type	i = 0;

	// 1. espacos antes de tudo nao contam
	skipBlanks(line, i);
	// 2. prefixo: so existe se a linha comeca com ':'. guardado sem o ':'
	if (i < line.size() && line[i] == ':')
	{
		++i;
		_prefix = nextWord(line, i);
		skipBlanks(line, i);
	}
	// 3. comando, normalizado para maiusculas: "privmsg" vira "PRIVMSG".
	// sem comando (linha vazia ou so espacos) _command fica vazio: isEmpty()
	_command = nextWord(line, i);
	for (std::string::size_type c = 0; c < _command.size(); ++c)
		_command[c] = static_cast<char>(std::toupper(static_cast<unsigned char>(_command[c])));
	// 4. parametros, um por palavra
	skipBlanks(line, i);
	while (i < line.size())
	{
		// trailing: um parametro que comeca com ':' leva o resto da linha
		// inteiro, espacos inclusive. o 15o parametro tambem, com ou sem ':'
		if (line[i] == ':' || _params.size() == MAX_PARAMS - 1)
		{
			if (line[i] == ':')
				++i;
			// pode ficar vazio ("PRIVMSG #a :"): o parametro existe, o
			// texto e que esta vazio, e o handler sabe a diferenca
			_params.push_back(line.substr(i));
			break ;
		}
		_params.push_back(nextWord(line, i));
		skipBlanks(line, i);
	}
}

Message::~Message()
{
}

// getters

const std::string	&Message::getPrefix() const
{
	return (_prefix);
}

const std::string	&Message::getCommand() const
{
	return (_command);
}

const std::vector<std::string>	&Message::getParams() const
{
	return (_params);
}

// sem comando nao ha o que fazer: linha vazia ou so com espacos, ou so um
// prefixo solto (":nick"). o RFC 2812 (2.3.1) manda ignorar mensagem vazia
// em silencio
bool	Message::isEmpty() const
{
	return (_command.empty());
}
