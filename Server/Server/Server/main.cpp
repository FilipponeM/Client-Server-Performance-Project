//Elliott Group Assignment 2
//Server 

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <iomanip> 
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

//define the packet
//something like
struct DataPacket {

    unsigned int clientID; //random int
    char fuelLevel[20];    // fuel level in gallons
    char timestamp[20]; //sec

};

void HandleClient(SOCKET clientSocket)
{
    cout << fixed << setprecision(6);

    DataPacket packet;
    float prevFuel = -1.0f;
    float firstFuel = -1.0f;
    float lastFuel = -1.0f;
    int prevTime = -1;
    int firstTime = -1;
    int lastTime = -1;
    float totalConsumption = 0.0f;
    int count = 0;
    unsigned int clientID = 0;

    auto parseTimeToSeconds = [](const char* timeStr) -> int
        {
            int day, month, year, hour, minute, second;
            if (sscanf(timeStr, " %d_%d_%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second) == 6)
                return hour * 3600 + minute * 60 + second;
            else
            {
                cerr << "[ERROR] Failed to parse time: " << timeStr << endl;
                return -1;
            }
        };

    while (true)
    {   
        //memset(&packet, 0, sizeof(packet));

        int bytesReceived = recv(clientSocket, (char*)&packet, sizeof(DataPacket), 0);

        cout << " Received: " << bytesReceived;

        if (bytesReceived <= 0)
            break;
        

        clientID = packet.clientID;

        float currentFuel = atof(packet.fuelLevel);
        int currentTime = parseTimeToSeconds(packet.timestamp);

        if (currentTime == -1)
            break;

        // Capture first valid time/fuel
        if (firstTime == -1)
        {
            firstTime = currentTime;
            firstFuel = currentFuel;
        }

        // For fuel consumption rate calculation
        if (prevFuel >= 0 && prevTime >= 0 && currentTime > prevTime)
        {
            float deltaFuel = prevFuel - currentFuel;
            float deltaTime = currentTime - prevTime;
            if (deltaTime > 0)
            {
                float rate = deltaFuel / deltaTime; // gallons per second
                totalConsumption += rate;
                count++;
            }
        }

        // Update tracking
        prevFuel = currentFuel;
        prevTime = currentTime;
        lastFuel = currentFuel;
        lastTime = currentTime;


    }

    //for now i want this to just show the clients id eery time that client sneds data
    //char buffer[1024] = { 0 };
    float avgGalPerSec = (count > 0) ? totalConsumption / count : 0.0f;
    float avgGph = avgGalPerSec * 3600.0f;
    float fuelUsed = (firstFuel >= 0 && lastFuel >= 0) ? (firstFuel - lastFuel) : 0.0f;
    int flightDurationSec = (firstTime >= 0 && lastTime >= 0) ? (lastTime - firstTime) : 0;

    //cout << "sent id" << endl;
    ofstream result("FlightResults.csv", ios::app);
    result << fixed << setprecision(6);
    result << "ClientID," << clientID
        << ",AvgFuelConsumption_GPH," << avgGph
        << ",TotalFuelUsed_Gal," << fuelUsed
        << ",FlightDuration_Sec," << flightDurationSec << endl;
    result.close();

    closesocket(clientSocket);

    cout << "\nConnection with Client-" << clientID << " closed." << endl;
    cout << "Total Flight Duration: " << flightDurationSec << " seconds" << endl;
    cout << "Total Fuel Used: " << fuelUsed << " gallons" << endl;
    cout << "Average Consumption: " << avgGph << " gallons/hour" << endl;
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
        //cout << "-1";
        ConnectionSocket = accept(ServerSocket, NULL, NULL);
        if (ConnectionSocket == INVALID_SOCKET) continue;

        //cout << "Client Connected!\n";
        thread clientThread(HandleClient, ConnectionSocket);

        clientThread.detach();

    }

    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}