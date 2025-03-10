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

Check the full documentation to dive deep into the project [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459) specially its more readable version (saved us a lot!) [Horse](https://modern.ircdocs.horse/).



## Getting Started
**Program overview:**
**TO BE UPDATED**
![image](https://github.com/user-attachments/assets/360bf9e6-a80e-4f8e-9587-71d3cc9b266c)


## Usage

If you want to run the project yourself:

	- First clone this git repository to your machine.
 
	- Enter the repo and run ``` make ```
 
	- The first time will download and set up the minilibx
 
	- Run ./ircserv <port> <password>

 	- Open the client (Hexchat) configure and connect to the server.


# Implementation

## Connection Analysis
