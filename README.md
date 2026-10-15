# Multithreaded Chat Server in C++

A multithreaded chat server built using C++ and TCP socket programming on Linux.

## Features
- TCP socket communication between clients and server.
- Supports multiple client connections using threads.
- Assigns a unique client ID to each connected client.
- Receives messages from multiple clients.

## Technologies Used
- C++
- Linux (Ubuntu via WSL)
- TCP/IP socket programming
- POSIX threads

## How It Works
1. The server creates a TCP socket.
2. It binds the socket to port 8080 and listens for connections.
3. When a client connects, the server assigns a client ID.
4. A separate thread handles communication with each client.
5. The server displays messages received from clients.

## How to Run

Compile the code:

```bash
g++ server.cpp -o server -pthread
```

Start the server:

```bash
./server
```

Connect using a client terminal:

```bash
nc 127.0.0.1 8080
```

Open additional terminals to connect more clients.

## Learning Outcomes
- TCP socket programming
- Multithreading in C++
- Client-server communication
- Linux development and GitHub version control
