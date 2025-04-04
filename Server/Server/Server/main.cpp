// Elliott Group Assignment 2
// Server
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <iomanip> // for std::fixed and std::setprecision
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct DataPacket
{
    unsigned int clientID; // random int
    char fuelLevel[16];    // fuel level in gallons
    char timestamp[20];    // Increased allocation to stop printing garbage
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

    auto parseTimeToSeconds = [](const char *timeStr) -> int
    {
        int day, month, year, hour, minute, second;
        if (sscanf(timeStr, "%d_%d_%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second) == 6)
            return hour * 3600 + minute * 60 + second;
        else
        {
            cerr << "[ERROR] Failed to parse time: " << timeStr << endl;
            return -1;
        }
    };

    while (true)
    {
        int bytesReceived = recv(clientSocket, (char *)&packet, sizeof(DataPacket), 0);
        if (bytesReceived <= 0)
            break;

        clientID = packet.clientID;
        float currentFuel = atof(packet.fuelLevel);
        int currentTime = parseTimeToSeconds(packet.timestamp);
        if (currentTime == -1)
            continue;

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

        cout << "Client ID: " << clientID
             << " | Fuel: " << currentFuel
             << " | Timestamp: " << packet.timestamp << endl;
    }

    float avgGalPerSec = (count > 0) ? totalConsumption / count : 0.0f;
    float avgGph = avgGalPerSec * 3600.0f;
    float fuelUsed = (firstFuel >= 0 && lastFuel >= 0) ? (firstFuel - lastFuel) : 0.0f;
    int flightDurationSec = (firstTime >= 0 && lastTime >= 0) ? (lastTime - firstTime) : 0;

    ofstream result("FlightResults.csv", ios::app);
    result << fixed << setprecision(6);
    result << "ClientID," << clientID
           << ",AvgFuelConsumption_GPH," << avgGph
           << ",TotalFuelUsed_Gal," << fuelUsed
           << ",FlightDuration_Sec," << flightDurationSec << endl;
    result.close();

    closesocket(clientSocket);

    cout << "Connection with Client-" << clientID << " closed." << endl;
    cout << "Total Flight Duration: " << flightDurationSec << " seconds" << endl;
    cout << "Total Fuel Used: " << fuelUsed << " gallons" << endl;
    cout << "Average Consumption: " << avgGph << " gallons/hour" << endl;
}

int main()
{
    WSADATA wsaData;
    SOCKET ServerSocket, ConnectionSocket;
    sockaddr_in SvrAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ServerSocket == INVALID_SOCKET)
    {
        cerr << "Failed to create server socket." << endl;
        return 1;
    }

    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(27001);

    if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
    {
        cerr << "Bind failed." << endl;
        return 1;
    }

    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "Listen failed." << endl;
        return 1;
    }

    cout << "Server is running. Waiting for connections..." << endl;

    while (true)
    {
        ConnectionSocket = accept(ServerSocket, NULL, NULL);
        if (ConnectionSocket == INVALID_SOCKET)
            continue;

        cout << "Client connected!" << endl;
        thread clientThread(HandleClient, ConnectionSocket);
        clientThread.detach();
    }

    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}
