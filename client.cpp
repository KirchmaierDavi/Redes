#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "Erro ao inicializar o Winsock." << endl;
        return 1;
    }
    
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "Erro ao criar o socket." << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Altere para o endereço do servidor

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    string message;
    while (true)
    {
        cout << "Digite uma mensagem ('quit' para sair): ";
        getline(cin, message);

        if (message == "quit")
        {
            break;
        }

        send(clientSocket, message.c_str(), message.length(), 0);

        char buffer[BUFFER_SIZE];
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            cerr << "Erro ao receber a mensagem de volta do servidor." << endl;
            break;
        }

        buffer[bytesRead] = '\0';
        cout << "Servidor diz: " << buffer << endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}