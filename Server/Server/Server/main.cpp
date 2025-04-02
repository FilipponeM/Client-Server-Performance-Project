//Elliott Group Assignment 2
//Server 

#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

//define the packet
//something like
struct DataPacket {

    unsigned int clientID; //random int
    string fuelLevel; //ml 
    string timeLeft; //sec

};

void HandleClient(SOCKET clientSocket) {

    //for now i want this to just show the clients id eery time that client sneds data
    char buffer[1024] = { 0 };

    cout << "sent id" << endl;

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET ServerSocket, ConnectionSocket;
    sockaddr_in SvrAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ServerSocket == INVALID_SOCKET) return -1;

    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(27001);

    if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) return -1;
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) return -1;

    cout << "Waiting for client connections...\n";

    while (true) {
        cout << "-1";
        ConnectionSocket = accept(ServerSocket, NULL, NULL);
        if (ConnectionSocket == INVALID_SOCKET) continue;

        cout << "Client Connected!\n";
        thread clientThread(HandleClient, ConnectionSocket);
        
        clientThread.detach();

    }

    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}