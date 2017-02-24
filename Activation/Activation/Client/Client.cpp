#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

char filename[] = "actFile.txt";
const int STRLEN = 256;

// Closes the socket and performs the WSACleanup
void cleanup(SOCKET socket);

int main()
{
	WSADATA		wsaData;
	SOCKET		mySocket;
	SOCKADDR_IN	serverAddr;
	char		machineID[STRLEN];
	char		serialNum[STRLEN];
	//char		sendMessage[STRLEN];
	char		recvMessage[STRLEN];
	//bool		done = false;
	char		ipAddress[] = "127.0.0.1";
	int			port = 50000;
	char		ID[STRLEN];

	cout << "Enter machineID: ";
	cin >> machineID;
	cin.ignore();

	//If data.txt does not exist then it is created otherwise it will add
	//machineID are added
	if (!ifstream(filename))
	{
		cout << "\nYou must activate this program in order to continue using it.\n";

		cout << "\nEnter your serial Number: ";
		cin >> serialNum;
		cin.ignore();

		// Loads Windows DLL (Winsock version 2.2) used in network programming
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		{
			cerr << "ERROR: Problem with WSAStartup\n";
			return 1;
		}

		// Create a new socket for communication
		mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (mySocket == INVALID_SOCKET)
		{
			cerr << "ERROR: Cannot create socket\n";
			WSACleanup();
			return 1;
		}

		cout << "\nConnecting to server...\n";

		// Setup a SOCKADDR_IN structure which will be used to hold address
		// and port information for the server. Notice that the port must be converted
		// from host byte order to network byte order.
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr);

		// Try to connect
		if (connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			cerr << "ERROR: Failed to connect\n";
			cleanup(mySocket);
			return 1;
		}

		int iSend = send(mySocket, serialNum, strlen(serialNum), 0);
		if (iSend == SOCKET_ERROR)
		{
			cerr << "\nERROR: Failed to send serial number\n";
			cleanup(mySocket);
			return 1;
		}

		int ok = recv(mySocket, recvMessage, STRLEN, 0);
		if (ok > 0)
		{
			recvMessage[ok] = '\0';
		}
		else if (ok == 0)
		{
			cout << "Connection closed\n";
			cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive serial number\n";
			cleanup(mySocket);
			return 1;
		}

		if (strcmp(recvMessage, "ok") == 0)
		{
			int machID = send(mySocket, machineID, strlen(machineID), 0);
			if (iSend == SOCKET_ERROR)
			{
				cerr << "\nERROR: Failed to send machine ID\n";
				cleanup(mySocket);
				return 1;
			}

			ok = recv(mySocket, recvMessage, STRLEN, 0);
			if (ok > 0)
			{
				recvMessage[ok] = '\0';
			}
			else if (ok == 0)
			{
				cout << "Connection closed\n";
				cleanup(mySocket);
				return 0;
			}
			else
			{
				cerr << "ERROR: Failed to receive message\n";
				cleanup(mySocket);
				return 1;
			}

			if (strcmp(recvMessage, "ok") == 0)
			{
				ofstream file(filename, ofstream::app);
				file << machineID;
				file << endl;
				file.close();

				cout << "\nActivation was successful!\n";
				return 0;
			}
			else if (strcmp(recvMessage, "no") == 0)
			{
				cout << "Activation was not successful.";
				return 0;
			}
			else
			{
				cerr << "ERROR: Failed to receive correct message.";
				cleanup(mySocket);
				return 1;
			}
		}
		else if (strcmp(recvMessage, "no") == 0)
		{
			cout << endl;
			cout << "Serial Number is Invalid.\n"; 
			cout << "Activation is not successful.\n";
			return 0;
		}
	}

	else
	{
		bool match = false;

		ifstream infile(filename, ifstream::in);
		while (!infile.eof() && !match)
		{
			infile.getline(ID, STRLEN);
			if (strcmp(machineID, ID) == 0)
			{
				infile.close();
				cout << "This program has already been activated. Thank you!\n";
				match = true;
				return 0;
			}
		}

		cout << endl;
		cout << "Activation data has been altered,\n";
		cout << "or the program has been copied to another machine.\n";
		cout << endl;

		cout << "You must activate this program in order to continue using it.\n";
		cout << endl;
		cout << "Enter your serial number: ";
		cin >> serialNum;
		cin.ignore();

		// Loads Windows DLL (Winsock version 2.2) used in network programming
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		{
			cerr << "ERROR: Problem with WSAStartup\n";
			return 1;
		}

		// Create a new socket for communication
		mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (mySocket == INVALID_SOCKET)
		{
			cerr << "ERROR: Cannot create socket\n";
			WSACleanup();
			return 1;
		}

		cout << "\nConnecting to server...\n";

		// Setup a SOCKADDR_IN structure which will be used to hold address
		// and port information for the server. Notice that the port must be converted
		// from host byte order to network byte order.
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr);

		// Try to connect
		if (connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			cerr << "ERROR: Failed to connect\n";
			cleanup(mySocket);
			return 1;
		}

		int iSend = send(mySocket, serialNum, strlen(serialNum), 0);
		if (iSend == SOCKET_ERROR)
		{
			cerr << "\nERROR: Failed to send serial number\n";
			cleanup(mySocket);
			return 1;
		}

		int ok = recv(mySocket, recvMessage, STRLEN, 0);
		if (ok > 0)
		{
			recvMessage[ok] = '\0';
		}
		else if (ok == 0)
		{
			cout << "Connection closed\n";
			cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive serial number\n";
			cleanup(mySocket);
			return 1;
		}

		if (strcmp(recvMessage, "ok") == 0)
		{
			int machID = send(mySocket, machineID, strlen(machineID), 0);
			if (iSend == SOCKET_ERROR)
			{
				cerr << "\nERROR: Failed to send machine ID\n";
				cleanup(mySocket);
				return 1;
			}

			ok = recv(mySocket, recvMessage, STRLEN, 0);
			if (ok > 0)
			{
				recvMessage[ok] = '\0';
			}
			else if (ok == 0)
			{
				cout << "Connection closed\n";
				cleanup(mySocket);
				return 0;
			}
			else
			{
				cerr << "ERROR: Failed to receive message\n";
				cleanup(mySocket);
				return 1;
			}

			if (strcmp(recvMessage, "ok") == 0)
			{
				ofstream file(filename, ofstream::app);
				file << machineID;
				file << endl;
				file.close();

				cout << endl << "Activation was successful!";
				return 0;
			}
			else if (strcmp(recvMessage, "no") == 0)
			{
				cout << endl << "Activation was not successful.";
				return 0;
			}
			else
			{
				cerr << "ERROR: Failed to receive correct message.";
			}
		}
		else if (strcmp(recvMessage, "no") == 0)
		{
			cout << "Program was previously activated on another machine./n";
			cout << "Activation was not successful.\n";
			return 0;
		}
		return 1;
	}
	return 0;
}

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}

