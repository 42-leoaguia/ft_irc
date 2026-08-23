*This project has been created as part of the 42 curriculum by anafonse, leoaguia, davmendo.*

# ft_irc

An IRC server written in C++98.

---

## Team

* <a href="https://github.com/liaFonseca"><img align="left" src="https://avatars.githubusercontent.com/liaFonseca?size=100" width="50"></a>
**[Lia Fonseca](https://github.com/liaFonseca)**
<br><br/>

* <a href="https://github.com/larocquel"><img align="left" src="https://avatars.githubusercontent.com/larocquel?size=100" width="50"></a>
**[Leonardo La Rocque](https://github.com/larocquel)**
<br><br/>

* <a href="https://github.com/davmendo"><img align="left" src="https://avatars.githubusercontent.com/davmendo?size=100" width="50"></a>
**[David Mendonça](https://github.com/davmendo)**
<br><br/>

---

## Description

**IRC (Internet Relay Chat)** is a text-based protocol for real-time messaging over
TCP/IP. Clients connect to a server, register with a nickname, and exchange messages
either privately or inside channels.

The goal of this project is to implement the **server side** of that protocol: a single
process, written in C++98, that accepts many simultaneous clients, understands a subset
of the IRC protocol as defined in RFC 1459 / RFC 2812, and works with a real,
unmodified IRC client.

The server handles user authentication, nicknames, channels, private and channel
messages, and the channel-operator commands. It is a single-threaded, event-driven
program: **one** `poll()` call drives every socket operation — listening, reading and
writing — and every file descriptor is set to non-blocking mode. No `fork()` is used,
and no blocking read or write is ever performed outside the `poll()` loop.

Out of scope by design, as required by the subject:

- no IRC client is implemented — an existing client is used to test the server;
- no server-to-server communication;
- no external libraries; the C++98 standard library and the authorized system calls only.

---

## Features

| Area | Commands |
|---|---|
| Registration | `PASS`, `NICK`, `USER`, `QUIT` |
| Connection keep-alive | `PING` / `PONG` |
| Messaging | `PRIVMSG`, `NOTICE` (to a user or to a channel) |
| Channels | `JOIN`, `PART`, `TOPIC` |
| Channel operators | `KICK`, `INVITE`, `TOPIC`, `MODE` |

Supported channel modes:

| Mode | Effect |
|---|---|
| `i` | invite-only channel |
| `t` | only operators may change the topic |
| `k` | channel key (password) |
| `o` | give / take channel operator privilege |
| `l` | set / remove the user limit |

Messages sent by a client to a channel are forwarded to every other client that joined
that channel. Both regular users and channel operators are supported.

**Reference client:** irssi.

---

## Instructions

### Requirements

- A Unix-like system (developed and tested on Ubuntu)
- `c++` (g++ or clang++) with C++98 support
- `make`
- An IRC client for testing (`irssi`, or `nc` for raw protocol testing)

### Build

```bash
make        # build ./ircserv
make clean  # remove object files
make fclean # remove object files and the binary
make re     # fclean + all
```

The project is compiled with `-Wall -Wextra -Werror` and complies with `-std=c++98`.

### Run

```bash
./ircserv <port> <password>
```

- `port` — the TCP port the server listens on (e.g. `6667`)
- `password` — the connection password every client must send with `PASS`

Example:

```bash
./ircserv 6667 mypassword
```

### Connect with the reference client

```bash
irssi
/connect localhost 6667 mypassword
/nick alice
/join #general
/msg #general hello everyone
/msg bob hi, this is a private message
```

### Connect with a raw client

```bash
nc -C 127.0.0.1 6667
PASS mypassword
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :hello
```

### Usage examples

```
/mode #general +i           # make the channel invite-only
/mode #general +k secret    # set a channel key
/mode #general +o bob       # promote bob to channel operator
/mode #general +l 10        # limit the channel to 10 users
/topic #general :new topic  # change the topic
/kick #general bob :reason  # eject bob from the channel
/invite bob #general        # invite bob to the channel
```

---

## Technical choices

**One `poll()`, non-blocking sockets.** Every descriptor — the listening socket included
— is registered in a single `pollfd` vector and set to non-blocking mode with
`fcntl(fd, F_SETFL, O_NONBLOCK)`. No `recv()` or `send()` is ever called on a descriptor
that `poll()` has not reported as ready.

**Per-client buffers and message framing.** TCP is a byte stream, not a message stream:
a single `recv()` may return half a command, or several commands at once. Each client
therefore owns an input buffer where incoming bytes are accumulated, and complete
commands are extracted only when a `\r\n` (or `\n`) delimiter is found. This is what
makes the partial-data test (`nc -C` with `ctrl+D`) work.

**Deferred writes.** Handlers never call `send()`. They append to the client's output
buffer, and the server flushes it only when `poll()` reports `POLLOUT` for that
descriptor. `POLLOUT` is requested only while there is something to send, so an idle
server does not spin.

**Command dispatch table.** The server holds a `std::map` from command name to member
function pointer. Adding a command means adding one source file and one registration
line — the network core never needs to know which commands exist.

**Ownership.** The server owns every `Client` and `Channel` object. Channels hold
non-owning pointers to clients; clients hold channel *names*, not pointers, so a
destroyed channel can never leave a dangling reference behind.

**Robustness.** The event loop catches exceptions, including `std::bad_alloc`, so that
no client action can terminate the process. `SIGINT` triggers a clean shutdown of every
open descriptor, and `SIGPIPE` is ignored so a client disconnecting mid-write cannot
kill the server.

---

## Project structure

```
ft_irc/
├── Makefile
├── README.md
├── CONTRIBUTING.md              internal conventions and interface contracts
├── inc/
│   ├── Server.hpp               event loop, sockets, dispatch table
│   ├── Client.hpp               per-client state and I/O buffers
│   ├── Channel.hpp              members, operators, modes, topic
│   ├── Message.hpp              IRC message parser
│   └── Replies.hpp              numeric reply macros (RFC 2812)
└── src/
    ├── main.cpp                 argument parsing, signal setup
    ├── core/
    │   ├── Server.cpp
    │   ├── Message.cpp
    │   ├── Client.cpp
    │   └── Channel.cpp
    └── commands/                one file per command
        ├── cmdPass.cpp   cmdNick.cpp    cmdUser.cpp
        ├── cmdPrivmsg.cpp cmdNotice.cpp cmdQuit.cpp  cmdPing.cpp
        ├── cmdJoin.cpp   cmdPart.cpp    cmdTopic.cpp
        └── cmdMode.cpp   cmdKick.cpp    cmdInvite.cpp
```

### Maintainers per component

| Component | Maintainer |
|---|---|
| `Server`, `Message`, `main.cpp`, `Makefile` | &lt;login1&gt; |
| `Client`, `PASS` `NICK` `USER` `PRIVMSG` `NOTICE` `QUIT` | &lt;login2&gt; |
| `Channel`, `JOIN` `PART` `TOPIC` `MODE` `KICK` `INVITE` | &lt;login3&gt; |
| `Replies.hpp` | shared |

Each component has one maintainer, but every part of the code was reviewed by at least
one other member before being merged.

---

## Testing

**Partial data.** The server must rebuild a command split across several packets:

```bash
nc -C 127.0.0.1 6667
com^Dman^Dd
```

(`ctrl+D` sends `com`, then `man`, then `d\n` — the server must process a single
`COMMAND`.)

**Other scenarios covered during development:**

- several clients connected at once, none blocking the others
- a client disconnecting in the middle of a command
- `SIGINT` on the server: every descriptor closed, no leak (`valgrind --leak-check=full`)
- wrong password, duplicate nickname, invalid nickname
- commands sent before registration is complete
- the last member leaving a channel (the channel is destroyed)
- messages larger than the read buffer

---

## Resources

### Protocol and network programming

- **RFC 2812** — *Internet Relay Chat: Client Protocol*. Message format (§2.3.1) and the
  full list of numeric replies (§5). The main reference for this project.
- **RFC 1459** — the original IRC specification, useful where RFC 2812 is ambiguous.
- **Beej's Guide to Network Programming** — sockets, `poll()`, and non-blocking I/O.
- `man` pages: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `poll(2)`, `fcntl(2)`,
  `recv(2)`, `send(2)`, `setsockopt(2)`, `sigaction(2)`.
- The `bircd` example provided with the subject — a minimal C event loop, used only to
  understand the general shape of a `select`/`poll` server before designing our own.

### Use of AI

> This section must be kept accurate as the project evolves. Update it whenever AI is
> used for something not listed here.

An AI assistant (Claude) was used during this project for the following tasks:

- **Project organisation** — building the roadmap, splitting the work between the three
  of us, defining milestones and the issue breakdown.
- **Interface design** — discussing the class layout (`Server`, `Client`, `Channel`,
  `Message`) and the responsibilities of each one, then producing header declarations
  (class members and method signatures with explanatory comments) that we used as a
  starting point. **No method body was generated: every implementation in `src/` was
  written by us.**
- **Documentation** — drafting this README and the internal conventions document.
- **Protocol clarification** — explaining which numeric replies apply to which error
  cases, always cross-checked against RFC 2812 before use.

AI was **not** used to generate the server implementation. Every design suggestion was
discussed, questioned and, where needed, corrected by the team; each file was reviewed
by a second member before merging, and every member is able to explain any part of the
code.
