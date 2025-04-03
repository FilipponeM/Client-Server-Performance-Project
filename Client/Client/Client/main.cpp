// Elliott Group Assignment 2
// Client

#define _CRT_SECURE_NO_WARNINGS
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

struct DataPacket
{
	unsigned int clientID;
	char fuelLevel[16]; // Enough for float string
	char timestamp[18]; // Enough for full timestamp string (e.g., "3_3_2023 15:54:21")
};

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cerr << "WSAStartup failed." << endl;
		return 1;
	}

	SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		cerr << "Failed to create socket." << endl;
		return 1;
	}

	string serverIP = (argc > 1) ? argv[1] : "127.0.0.1";

	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(27001);
	SvrAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

	if ((connect(ClientSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(ClientSocket);
		WSACleanup();
		cerr << "Connection failed." << endl;
		return 1;
	}

	DataPacket datapacket;

	srand((unsigned)time(0));
	datapacket.clientID = (rand() % 10 + 1) + GetCurrentProcessId(); // Stronger random ID using rand(1 ~ 10) + process ID)

	int roll = rand() % 4 + 1; // Randomly pick a file 1 to 4
	string filename;

	switch (roll)
	{
	case 1:
		filename = "Data Files/katl-kefd-B737-700.txt";
		break;
	case 2:
		filename = "Data Files/Telem_2023_3_12 14_56_40.txt";
		break;
	case 3:
		filename = "Data Files/Telem_2023_3_12 16_26_4.txt";
		break;
	case 4:
		filename = "Data Files/Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt";
		break;
	default:
		cerr << "Invalid roll." << endl;
		return 1;
	}

	ifstream file(filename);
	if (!file.is_open())
	{
		cerr << "Failed to open file: " << filename << endl;
		return 1;
	}

	string line;
	bool isFirstLine = true;

	while (getline(file, line))
	{
		if (isFirstLine)
		{
			isFirstLine = false;
			continue; // Skip header
		}

		stringstream ss(line);
		string timeStr, fuelStr;
		getline(ss, timeStr, ',');
		getline(ss, fuelStr, ',');

		strncpy(datapacket.timestamp, timeStr.c_str(), sizeof(datapacket.timestamp));
		strncpy(datapacket.fuelLevel, fuelStr.c_str(), sizeof(datapacket.fuelLevel));

		cout << "Sent packet: ID = " << datapacket.clientID
			 << ", Fuel = " << datapacket.fuelLevel
			 << ", Time = " << datapacket.timestamp << endl;

		send(ClientSocket, (char *)&datapacket, sizeof(datapacket), 0);
		Sleep(500);
	}

	file.close();
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}
