This project was done by:

[Aija Arepsa](https://github.com/AijaRe)

[Yasmine Fontenele](https://github.com/yasminefontenele)

[Uatilla Almeida](https://github.com/Uatilla)

# FT_IRC
![clideo_editor_c549726371ed491a84bd207847e31387](https://github.com/user-attachments/assets/6eef85c3-f44e-4016-82ed-67bc8510b6f8)

**FT_IRC** is a C project developed as part of the 42 School Common Core Program. It is a simplified implementation of an IRC (Internet Relay Chat) server that adheres to the IRC protocol (RFC 1459). The primary objectives of this project are to:
 
    *Handle multiple clients connected to the server simultaneously.
    *Messages addressed to a channel must be forwarded to every other client that joined the channel.
    *Implement the following IRC commands: 
      /NICK - Set a nickname.
      /USER - Set an username.
      /JOIN - Join a channel.
      /PRIVMSG - Send a message to a target (Channel or User).
      /INVITE - Invite a client to a channel.
      /KICK - Eject a client from a channel.
      /TOPIC - Change of view the channel topic.
      /MODE - Change the channels mode:
        i: Set/remove invite-only channel.
        t: Set/remove the restrictions of the TOPIC command to channel operators.
        k: Set/remove requirement to join a channel with a password.
        o: Give/take operator priviliges to a client.
        l: Set/remove the user limit to channel. 
    *Adhere to the IRC protocol standards for interoperability with other IRC clients and servers.

For more detailed information about the challenge, look at the [**subject of this project**](https://cdn.intra.42.fr/pdf/pdf/153311/en.subject.pdf).

## What IRC means?

**IRC** (Internet Relay Chat) is a protocol for real-time text messaging and communication. It was developed in 1988 and remains a foundational technology for online chat systems. IRC enables users to connect to a server, join channels, and communicate with others through text messages. It supports features like private messaging, channel moderation, and user authentication, making it a versatile tool for group discussions and community building.

An IRC server, in general, acts as the central hub for communication. It manages connections from multiple clients, routes messages between users, and enforces rules for channels and user interactions. Clients connect to the server using IRC commands to perform actions like setting a nickname, joining channels, sending messages, and disconnecting.

Check the full documentation to dive deep into the project [RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812) specially its more readable version (saved us a lot!) [Horse: Modern IRC Client Protocol](https://modern.ircdocs.horse/).

### Steps overview:
![image](https://github.com/user-attachments/assets/fd47269a-300b-401d-aa16-2a1b2cd87725)

## Getting Started
**Program overview:**
![image](https://github.com/user-attachments/assets/7793effa-d223-4318-afdb-14763104904a)




## Usage

If you want to run the project yourself:

	- First clone this git repository to your machine.
 
	- Enter the repo and run ``` make ```
 
	- The first time will download and set up the minilibx
 
	- Run ./ircserv <port> <password>

 	- Open the client (Hexchat) configure and connect to the server.


# Implementation

## Connection Management
Poll

## Command Processing
| Command | Description | Format |
|-------------|------------|--------|
| CAP | Capability negotiation | `/CAP LS`<br>`/CAP END`<br>Our server responds to LS with empty capabilities |
| INVITE | The `INVITE` command is used to invite a user to a channel. | `/INVITE <nickname> <channel>` |
| JOIN | The `JOIN` command indicates that the client wants to join the given channel(s), each channel using the given key for it. | `/JOIN #channel1,#channel2,#channel3 pass1,pass2` |
| KICK | The `KICK` command can be used to request the forced removal of a user from a channel. | `/KICK <channel> <nickname> <optional_reason>` |
| MODE | The `MODE` command is used to set or remove options (or modes) from a given target. <br>Our user modes: `o`.<br>Our channel modes: `i, k, l, t`<br> `+i` - invite only<br>`+k` - add password/key to channel<br>`+l` - set user limit for a channel<br>`+t` - protect topic to be modified only by operators | `/MODE <channel> <mode_parameter>`<br>`/MODE` - to see current channel modes |
| MOTD | The `MOTD` command is used to get the “Message of the Day” of the given server. | Our server sends MOTD to client only after authentication, not callable. |
| NICK | The `NICK` command is used to give the client a nickname or change the previous one. | `/NICK <new_nickname>` |
| NOTICE | The `NOTICE` command is used to send notices between users, as well as to send notices to channels. The difference between `NOTICE` and `PRIVMSG` is that automatic replies must never be sent in response to a `NOTICE` message. | `/NOTICE <target> <message>` |
| PART | The `PART` command removes the client from the given channel(s). | `/PART <channel>` |
| PASS | The `PASS` command is used to set a ‘connection password’ to the server. If set, the password must be set before any attempt to register the connection is made. | `/PASS <password>` |
| PING | The `PING` command is sent by either clients or servers to check the other side of the connection is still connected and/or to check for connection latency, at the application layer. | `/PING <token>` |
| PRIVMSG | The `PRIVMSG` command is used to send private messages between users, as well as to send messages to channels. | `/PRIVMSG <target> <message>` |
| QUIT | The `QUIT` command is used to terminate a client’s connection to the server. The server acknowledges this by replying with an `ERROR` message and closing the connection to the client. | `/QUIT <optional_message>` |
| TOPIC | The `TOPIC` command is used to change or view the topic of the given channel. | `/TOPIC <channel> <new_topic>` |
| USER | The `USER` command is used at the beginning of a connection to specify the username and real name of a new user. | `/USER <username> 0 * <realname>` |


Signals
Modifications on the data structure

## Server Output
"Feedback" to the client if success or not with the command process.


