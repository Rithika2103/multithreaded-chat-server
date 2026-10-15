
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

mutex clientsMutex;
vector<pair<int, int>> clients;
int nextClientId = 1;

void broadcastMessage(const string& message) {
    lock_guard<mutex> lock(clientsMutex);

    for (const auto& client : clients) {
        send(client.first, message.c_str(), message.size(), 0);
    }
}

void handleClient(int clientSocket, int clientId) {
    char buffer[1024];

    while (true) {
        int bytesReceived = recv(
            clientSocket, buffer, sizeof(buffer), 0
        );

        if (bytesReceived <= 0) {
            break;
        }

        string message(buffer, bytesReceived);

        // Add a newline if the client did not send one.
        if (message.empty() || message.back() != '\n') {
            message += '\n';
        }

        string formattedMessage =
            "Client " + to_string(clientId) + ": " + message;

        cout << formattedMessage;
        broadcastMessage(formattedMessage);
    }

    {
        lock_guard<mutex> lock(clientsMutex);

        clients.erase(
            remove_if(clients.begin(), clients.end(),
                [clientSocket](const pair<int, int>& client) {
                    return client.first == clientSocket;
                }),
            clients.end()
        );
    }

    close(clientSocket);

    string leaveMessage =
        "Client " + to_string(clientId) + " disconnected.\n";

    cout << leaveMessage;
    broadcastMessage(leaveMessage);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&serverAddress,
             sizeof(serverAddress)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        close(serverSocket);
        return 1;
    }

    cout << "Chat server listening on port 8080...\n";

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddress,
            &clientLength
        );

        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }

        int clientId;

        {
            lock_guard<mutex> lock(clientsMutex);
            clientId = nextClientId++;
            clients.push_back({clientSocket, clientId});
        }

        cout << "Client " << clientId << " connected.\n";

        broadcastMessage(
            "Client " + to_string(clientId) + " joined the chat.\n"
        );

        thread(handleClient, clientSocket, clientId).detach();
    }

    close(serverSocket);
    return 0;
}
