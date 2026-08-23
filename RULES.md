# Regras do time

**Início:** 27/08 · **Defesa:** 02/09

---
## 1. Padrão de commit:

- `feat: Criação de um novo recurso.`
- `fix: Correção de um erro ou bug.`
- `docs: Mudanças apenas na documentação.`
- `refactor: Alteração de código que não corrige bugs nem adiciona recursos.`
- `test: Adição ou correção de testes.`
- `chore: Atualizações de tarefas de rotina, build ou dependências`

---

## 2. Estrutura Dividida
```
ft_irc/
├── Makefile                        → Léo
├── README.md                       → Léo
├── .gitignore
├── inc/
│   ├── Server.hpp                  → Léo
│   ├── Client.hpp                  → David
│   ├── Channel.hpp                 → Lia
│   ├── Message.hpp                 → Léo
│   └── Replies.hpp                 → os 3
|
└── src/
    ├── main.cpp                    → Léo
    ├── core/
    │   ├── Server.cpp              → Léo   — poll loop, accept, recv/send, dispatch
    │   ├── Message.cpp             → Léo   — parser IRC (prefix/command/params)
    │   ├── Client.cpp              → David — nick, user, auth, buffers, prefix()
    │   └── Channel.cpp             → Lia   — membros, ops, modos, topic, broadcast
    |
    └── commands/           # 1 arquivo = 1 comando = 1 issue = 1 PR
        ├── cmdPass.cpp     ┐
        ├── cmdNick.cpp     │
        ├── cmdUser.cpp     ├─ David
        ├── cmdPrivmsg.cpp  │
        ├── cmdNotice.cpp   │
        ├── cmdQuit.cpp     ┘
        ├── cmdJoin.cpp     ┐
        ├── cmdPart.cpp     │
        ├── cmdTopic.cpp    ├─ Lia
        ├── cmdMode.cpp     │
        ├── cmdKick.cpp     │
        ├── cmdInvite.cpp   ┘
        └── cmdPing.cpp     → Léo
```

Precisou de algo no arquivo de outra pessoa? Fale antes. Não edite por conta própria.

**`Replies.hpp` é append-only:** só acrescente linhas no fim. Nunca reordene nem
reformate — é o único arquivo que os três tocam, e assim o conflito fica trivial.

**Um comando = um arquivo = uma issue = um PR.** O `Server` não conhece nome de comando
nenhum: existe uma `std::map<std::string, CommandHandler>` preenchida em
`Server::initCommands()`. Comando novo = arquivo novo + **uma** linha lá.

---

## 3. Regras do subject que valem nota 0

Causas de nota 0:

- **Crash em qualquer circunstância = 0**, inclusive falta de memória. O loop principal
  tem `try/catch`, e `std::bad_alloc` é tratado. Nada de `assert` ou `exit()` no meio do
  código.
- **`recv`/`send` em qualquer fd sem passar pelo `poll()` = 0.** Mesmo que o fd seja
  não-bloqueante e "funcione". Só `Server::readFrom()` e `Server::writeTo()` tocam a rede.
- **`fork()` é proibido.** Um processo, uma thread, um `poll()`.
- **Um único `poll()`** para tudo: listen, read e write.
- **O cliente de referência (irssi) precisa conectar sem nenhum erro na tela.**
- **Bônus só é avaliado se o obrigatório estiver PERFEITO.** Na prática: só pensamos em
  bônus se sobrar tempo depois de tudo testado.

---

## 4. Contratos congelados

> Definidos no dia 27.
**Não mudam sem os três concordarem.**

### 4.1 `Message` — o que o parser devolve

```cpp
std::string                 _prefix;   // sem os ':' — vazio se o cliente não mandou
std::string                 _command;  // sempre em MAIÚSCULAS
std::vector<std::string>    _params;   // o trailing é o ÚLTIMO elemento
```

- O comando é normalizado para maiúsculas pelo parser: handlers nunca comparam
  case-insensitive.
- O *trailing* (tudo depois do `:` que inicia um parâmetro) entra inteiro como último
  elemento de `_params`, sem os dois-pontos e podendo conter espaços.
- Linhas vazias são descartadas pelo parser.
- O parser **não valida** número de parâmetros — quem responde `461` é o handler.

```
PRIVMSG #geral :ola pessoal, tudo bem?
  _command = "PRIVMSG"
  _params  = ["#geral", "ola pessoal, tudo bem?"]
```

### 4.2 Como um comando responde — `Client::queue()`

```cpp
void Client::queue(const std::string& msg);   // acrescenta o "\r\n" sozinho
```

Enfileira no buffer de saída e sinaliza `POLLOUT`.
**Nenhum comando chama `send()`.** Isso é regra de nota 0, não estilo.

### 4.3 Como falar com um canal — `Channel::broadcast()`

```cpp
void Channel::broadcast(const std::string& msg, Client* except);
```

`except != NULL` pula esse cliente — use no `PRIVMSG`.
`except == NULL` manda para todos, inclusive o autor — `JOIN`, `PART`, `KICK`, `MODE`,
`TOPIC` (o cliente precisa ver a confirmação do próprio comando).

### 4.4 Formato do prefixo — `Client::prefix()`

```cpp
std::string Client::prefix() const;   // ":nick!user@host"
```

Mensagens repassadas a outros clientes levam o prefixo do autor:

```
:leo!leo@localhost PRIVMSG #geral :ola
```

Respostas numéricas usam o nome do servidor e incluem o nick do destinatário:

```
:ircserv 433 * david :Nickname is already in use
```

---

## 5. Fluxo de trabalho

### Branches

```
feat/<nº-da-issue>-<nome-curto>     feat/14-join-command
fix/<nº-da-issue>-<nome-curto>      fix/23-partial-buffer
```

`main` protegida: sem push direto, PR obrigatório com **1 aprovação**, e o job de build
do CI como *required status check*.

### Checklist de PR

- [ ] Compila com `-Wall -Wextra -Werror -std=c++98` sem nenhum warning
- [ ] Testado com `nc -C` **e** com o irssi
- [ ] Testado o caminho de erro (parâmetro faltando, sem permissão, alvo inexistente)
- [ ] `valgrind --leak-check=full` sem leak novo
- [ ] Não chamei `send()` fora do `Server`
- [ ] Não editei arquivo de outro dono

### Review

O revisor **nunca é o autor**, e a função rotaciona.

---

## 6. Milestones

| # | Milestone | Prazo | Critério de fechamento |
|---|---|---|---|
| **M1** | Núcleo de rede | 28/08 | `nc -C` com `^D` monta o comando; 3 clientes simultâneos; disconnect não crasha |
| **M2** | Registro | 29/08 | irssi conecta **sem nenhum erro** na tela |
| **M3** | Canais e mensagens | 30/08 | 2 clientes conversam em canal e em DM |
| **M4** | Operadores e modos | 31/08 | cada modo testado como op **e** como usuário comum |
| **M5** | Robustez e defesa | 02/09 | zero leak, zero crash, os 3 explicam o código inteiro |

M1 tem os três na mesma tela (mob programming), sem PR. Se M1 estourar o dia 28, o corte
é `MODE o` e `MODE l` primeiro — nunca o M5.

---

## 7. Preparação para a defesa

Ensaiar no dia 02, cada um por vez:

1. Acrescentar um comando novo do zero em menos de 10 minutos.
2. Explicar por que `Client::extractLine()` existe, em vez de tratar cada `recv()` como
   um comando.
3. Explicar por que o `POLLOUT` só é pedido quando há bytes na fila — e o que aconteceria
   com a CPU se ficasse sempre ligado.
4. Explicar por que `Channel` guarda `Client*` e `Client` guarda `std::string` de canais.
5. Mostrar onde está garantido que nenhum `send()` acontece fora do `poll()`.

E o README precisa estar atualizado!
