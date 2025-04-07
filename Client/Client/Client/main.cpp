// Elliott Group Assignment 2
// Client



#define _CRT_SECURE_NO_WARNINGS
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cerrno>
#include <cstdlib>  
#include <ctime>

using namespace std;

//define the packet
//something like
struct DataPacket {

	unsigned int clientID; //random int
	char fuelLevel[24]; //ml 
	char timeLeft[24]; //sec

};


int main()
{
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27001);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address


	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return 0;
	}

	DataPacket datapacket;

	//set a unique id number 
	srand((unsigned)time(0));
	datapacket.clientID = (rand() % 10 + 1) + GetCurrentProcessId(); // Stronger random ID using rand(1 ~ 10) + process ID)

	int roll = rand() % 4 + 1; // Randomly pick a file 1 to 4
	string filename;

	if (roll == 1) {
		cout << "1" << endl;
		filename = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\katl-kefd-B737-700.txt";
		
	}
	else if (roll == 2) {
		cout << "2" << endl;
		filename = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_2023_3_12 14_56_40.txt";
		
	}
	else if (roll == 3) {
		cout << "3" << endl;
		filename = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_2023_3_12 16_26_4.txt";
		
	}
	else if (roll == 4) {
		cout << "4" << endl;
		filename = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt";
		
	}
	else {
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	//testing
	cout << filename << endl;


	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Failed to open file: " << filename << endl;
		cerr << "Error: " << strerror(errno) << endl;
		return 1;  // Exit the program if the file cannot be opened
	}

	string line;
	
	bool isFirstLine = true; 

	//while loop till file is done
	while (getline(file, line)) {

		vector<string> values;
		stringstream ss(line);
		string value;
		

		// Split the line by commas
		while (getline(ss, value, ',')) {
			values.push_back(value);
		}

		cout << values.size() << endl;

		//check for 3 because of the extra comma in the txt files
		if (values.size() == 3) {
			try {

				strncpy(datapacket.timeLeft, values[0].c_str(), sizeof(datapacket.timeLeft) - 1);
				datapacket.timeLeft[sizeof(datapacket.timeLeft) - 1] = '\0';

				strncpy(datapacket.fuelLevel, values[1].c_str(), sizeof(datapacket.fuelLevel) - 1);
				datapacket.fuelLevel[sizeof(datapacket.fuelLevel) - 1] = '\0';
				  
				cout << "Time Level: " << datapacket.timeLeft
					<< ", Fuel Left: " << datapacket.fuelLevel << endl;
			}
			catch (const std::invalid_argument& e) {
				cerr << "Invalid data in line: " << line << endl;
			}


		}
		else if (values.size() == 4 && isFirstLine==true) {
			try {

				strncpy(datapacket.timeLeft, values[1].c_str(), sizeof(datapacket.timeLeft) - 1);
				datapacket.timeLeft[sizeof(datapacket.timeLeft) - 1] = '\0';

				strncpy(datapacket.fuelLevel, values[2].c_str(), sizeof(datapacket.fuelLevel) - 1);
				datapacket.fuelLevel[sizeof(datapacket.fuelLevel) - 1] = '\0';

				cout << "Time Level: " << datapacket.timeLeft
					<< ", Fuel Left: " << datapacket.fuelLevel << endl;
			}
			catch (const std::invalid_argument& e) {
				cerr << "Invalid data in line: " << line << endl;
			}

			isFirstLine = false;
		}
		else {
			cerr << "Insufficient data in line: " << line << endl;
		}

		//packet the data
		//send data to server

		send(ClientSocket, (char*)&datapacket, sizeof(datapacket), 0);

	}

	//close file
	file.close();

	//clean up socket use
	closesocket(ClientSocket);
	WSACleanup();

	return 1;

}


