/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 18:52:15 by leoaguia          #+#    #+#             */
/*   Updated: 2026/09/02 03:38:55 by leoaguia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// main.cpp

#include <iostream>   // std::cerr, std::endl
#include <string>     // std::string
#include <stdexcept>  // std::invalid_argument, std::exception

// #include "Server.hpp"   // TODO: descomentar quando a issue #3 entrar na main

// Valida a senha: Não pode estar vazia
static void	validatePass(const std::string& pass)
{
	if (pass.empty())
		throw std::invalid_argument("password must not be empty");
}

// Valida e converte a porta:
// - Somente dígitos
// - 1 a 5 dígitos
// - Valor entre 1024 e 65535 (0~1023 = well-known ports e 65535 = limite 16 bits)
static int	validatePort(const std::string& port)
{
	int	value;

	// 1. Porta vazia
	if (port.empty())
		throw std::invalid_argument("port must not be empty");

	// 2. Somente dígitos
	for (std::string::size_type i = 0; i < port.size(); ++i)
	{
		if (port[i] < '0' || port[i] > '9')
			throw std::invalid_argument("port must contain digits only: " + port);
	}

	// 3. 1 a 5 dígitos
	if (port.size() > 5)
		throw std::invalid_argument("port is too long: " + port);


	// 4. Conversão (string -> int)
	value = 0;
	for (std::string::size_type i = 0; i < port.size(); ++i)
	{
		value = value * 10 + (port[i] - '0');
	}


	// 5. Intervalo correto (0~1023 não pode e nem maiores que 16 bit)
	if (value < 1024 || value > 65535)
		throw std::invalid_argument("port must be between 1024 and 65535");

	return (value);
}

// Validação dos argumentos e ponto de entrada do servidor.
int	main(int argc, char **argv)
{
	// 1. Ensinar ao usuário
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	try
	{
		// 2. Valida os argumentos. Qualquer problema vira exceção e cai no catch.
		int			port = validatePort(argv[1]);

		std::string	pass = argv[2];

		validatePass(pass);

		// 2. Constrói o Server: socket, bind, listen. Lança se algum falhar.
		// Server	server(port, pass);

		// 3. Entra no loop de poll(). Só retorna no SIGINT.
		// server.run()

		// TODO: remover quando o Server existir (issue #3)
		std::cout	<< "arguments ok: port " << port
					<< ", pass size " << pass.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		// Um único ponto de saída para erro. Capturar por referência a
		// std::exception pega invalid_argument, runtime_error, bad_alloc e
		// qualquer exceção padrão que o Server venha a lançar, sem listar
		// cada tipo aqui.
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}