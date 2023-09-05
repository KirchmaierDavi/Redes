#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 8080;
const int MAX_CONNECTIONS = 5;
const int BUFFER_SIZE = 1024;

mutex cout_mutex;

void handleClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while (true)
    {
        bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            break;
        }

        buffer[bytesRead] = '\0';
        string command(buffer);

        cout << "Cliente diz: " << command << endl;

        send(clientSocket, command.c_str(), command.length(), 0);
        if (command.substr(0, 4) == "quit")
        {
            break;
        }
    }

    closesocket(clientSocket);
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "Erro ao inicializar o Winsock." << endl;
        return 1;
    }

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "Erro ao criar socket." << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Erro ao ligar o socket." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR)
    {
        cerr << "Erro ao colocar o socket em modo de escuta." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Servidor escutando na porta " << PORT << "..." << endl;

    while (true)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            cerr << "Erro ao aceitar conexão." << endl;
            continue;
        }

        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Cliente conectado: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << endl;
        }

        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
