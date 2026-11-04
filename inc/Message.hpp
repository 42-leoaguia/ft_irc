/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davmendo <davmendo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:02:07 by leoaguia          #+#    #+#             */
/*   Updated: 2026/10/06 12:56:37 by davmendo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstddef>	// size_t
#include <string>	// std::string
#include <vector>	// std::vector

/*
uma linha do IRC ja separada nas suas partes (RFC 2812, 2.3.1):

	[":" prefixo] COMANDO [parametros ...] [":" trailing]

o Server cria uma Message para cada linha que Client::extractLine() devolve
e o handler do comando so le os campos. contrato 4.1 do RULES.md:
- _prefix: sem o ':', vazio se o cliente nao mandou;
- _command: sempre em MAIUSCULAS, os handlers nunca comparam sem caixa;
- _params: o trailing e o ULTIMO elemento, inteiro e sem o ':'.

o parser nao valida o numero de parametros: quem responde 461 e o handler.

	PRIVMSG #geral :ola pessoal, tudo bem?
	_command = "PRIVMSG"
	_params  = ["#geral", "ola pessoal, tudo bem?"]
*/
class Message
{
public:
	// RFC 2812: no maximo 15 parametros. o 15o leva o resto da linha inteiro,
	// mesmo sem ':'
	static const size_t	MAX_PARAMS = 15;

	// line: uma linha ja sem o "\r\n", como extractLine() devolve
	explicit Message(const std::string &line);
	~Message();

	// getters
	const std::string				&getPrefix() const;
	const std::string				&getCommand() const;
	const std::vector<std::string>	&getParams() const;

	// true quando a linha nao tem comando (vazia ou so com espacos).
	// o Server descarta essas linhas sem responder nada
	bool							isEmpty() const;

private:
	std::string					_prefix;	// sem o ':'. vazio se nao veio
	std::string					_command;	// sempre em maiusculas
	std::vector<std::string>	_params;	// o trailing e o ultimo
};

#endif
