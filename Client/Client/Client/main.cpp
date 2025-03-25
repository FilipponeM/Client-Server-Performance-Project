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
	string fuelLevel; //ml 
	string timeLeft; //sec

};


int main()
{
	////starts Winsock DLLs
	//WSADATA wsaData;
	//if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
	//	return 0;
	//}

	////initializes socket. SOCK_STREAM: TCP
	//SOCKET ClientSocket;
	//ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if (ClientSocket == INVALID_SOCKET) {
	//	WSACleanup();
	//	return 0;
	//}

	////Connect socket to specified server
	//sockaddr_in SvrAddr;
	//SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	//SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	//SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address


	//if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
	//	closesocket(ClientSocket);
	//	WSACleanup();
	//	return 0;
	//}

	DataPacket datapacket;

	//set a unique id number 
	datapacket.clientID = 1;

	//open file randomly for testing/dont know what he wants
	srand(static_cast<unsigned>(time(0)));
	int roll = rand() % 4 + 1;

	string name;

	if (roll == 1) {
		cout << "1" << endl;
		name = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\katl-kefd-B737-700.txt";
		
	}
	else if (roll == 2) {
		cout << "2" << endl;
		name = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_2023_3_12 14_56_40.txt";
		
	}
	else if (roll == 3) {
		cout << "3" << endl;
		name = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_2023_3_12 16_26_4.txt";
		
	}
	else if (roll == 4) {
		cout << "4" << endl;
		name = "C:\\Users\\filip\\Desktop\\client-server-elliott\\Data Files\\Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt";
		
	}
	else {
		/*closesocket(ClientSocket);
		WSACleanup();*/
		return -1;
	}

	//testing
	cout << name << endl;


	ifstream file(name);
	if (!file.is_open()) {
		cerr << "Failed to open file: " << name << endl;
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

				datapacket.timeLeft = values[0];
				datapacket.fuelLevel = values[1];
				  
				cout << "Time Level: " << datapacket.timeLeft
					<< ", Fuel Left: " << datapacket.fuelLevel << endl;
			}
			catch (const std::invalid_argument& e) {
				cerr << "Invalid data in line: " << line << endl;
			}


		}
		else if (values.size() == 4 && isFirstLine==true) {
			try {

				datapacket.timeLeft = values[1];
				datapacket.fuelLevel = values[2];

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






	}

		//packet the data
		//send data to server

	

	//close file
	file.close();

	/*closesocket(ClientSocket);
	WSACleanup();*/


	return 1;

}


