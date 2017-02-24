#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <Windows.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

char filename[] = "data.txt";
const int STRLEN = 256;

// Closes the socket and performs the WSACleanup
void cleanup(SOCKET socket);

int main()
{
	int			port = 50000;
	WSADATA		wsaData;
	SOCKET		listenSocket;
	SOCKET		acceptSocket;
	SOCKADDR_IN	serverAddr;
	char		recvMessage[STRLEN];
	bool		done = false;
	int         isValid = 0;
	char        validStatus[STRLEN] = "ok";
	char        serialNum[STRLEN];
	char        machineID[STRLEN];
	char        invalidStatus[STRLEN] = "no";
	char        checkSerial[STRLEN];
	char        checkID[STRLEN];
	bool        isNew = false;
	bool        matchFound = false;
	bool        foundSerial = false;
	bool        foundID = false;


	if (!ifstream(filename))
	{
		ofstream outFile;
		outFile.open(filename);
		isNew = true;
	}
	else
	{
		ifstream inFile(filename, ifstream::in);
		isNew = false;
	}

	// Loads Windows DLL (Winsock version 2.2) used in network programming
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		cerr << "ERROR: Problem with WSAStartup\n";
		return 1;
	}

	// Create a new socket to listen for client connections
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "ERROR: Cannot create socket\n";
		WSACleanup();
		return 1;
	}

	// Setup a SOCKADDR_IN structure which will be used to hold address
	// and port information. Notice that the port must be converted
	// from host byte order to network byte order.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	// Attempt to bind to the port.
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cerr << "ERROR: Cannot bind to port\n";
		cleanup(listenSocket);
		return 1;
	}

	cout << "Running on port number 50000\n";

	// Start listening for incoming connections
	if (listen(listenSocket, 1) == SOCKET_ERROR)
	{
		cerr << "ERROR: Problem with listening on socket\n";
		cleanup(listenSocket);
		return 1;
	}

	while (!done)
	{
		isNew = false;
		matchFound = false;
		foundSerial = false;
		foundID = false;
		done = false;

		cout << "\nWaiting for connections...\n";

		// Accept incoming connection. Program pauses here until
		// a connection arrives.
		acceptSocket = accept(listenSocket, NULL, NULL);

		if (acceptSocket == INVALID_SOCKET)
		{
			cerr << "ERROR: Cannot create socket\n";
			WSACleanup();
			return 1;
		}

		int Receive = recv(acceptSocket, recvMessage, STRLEN, 0); // Receive data from client

		if (Receive > 0) // No errors occurred
		{
			recvMessage[Receive] = '\0';
		}
		else if (Receive == 0) // Connection was closed
		{
			cout << "Connection closed\n";
			closesocket(listenSocket);
			cleanup(acceptSocket);
			return 0;
		}
		else // An error has occurred
		{
			cerr << "ERROR: Failed to receive message\n";
			closesocket(listenSocket);
			cleanup(acceptSocket);
			return 1;
		}

		// Iterate through the received message and insure each char is a number
		// if it isn't a number, flag it with the isValid flag.
		for (int con = 0; con < strlen(recvMessage); con++)
		{
			if (!isdigit(recvMessage[con]))
			{
				isValid = 1;
			}
		}

		if (isValid == 0) // Everything went alright
		{
			strncpy_s(serialNum, recvMessage, sizeof(serialNum));

			// Send whether the serial number was valid or invalid to the client
			int serial = send(acceptSocket, validStatus, strlen(validStatus), 0);

			if (serial == SOCKET_ERROR)
			{
				cerr << "ERROR: Failed to send serialNum\n";
				closesocket(listenSocket);
				cleanup(acceptSocket);
				return 1;
			}
		}
		else // If something was found wrong with the serial number
		{
			cout << endl;
			cout << "SerialNum: INVALID" << endl;
			
			// Send whether the serial number was valid or invalid to the client
			int serial = send(acceptSocket, invalidStatus, strlen(invalidStatus), 0);

			if (serial == SOCKET_ERROR)
			{
				cerr << "ERROR: Failed to send serialNum\n";
				closesocket(listenSocket);
				cleanup(acceptSocket);
				return 1;
			}
		}

		// If it wasn't valid, close the connection and start waiting for another
		if (isValid == 1)
		{
			closesocket(acceptSocket);
		}

		else
		{
			int Receive = recv(acceptSocket, recvMessage, STRLEN, 0); // Receive data from client

			if (Receive > 0) // No errors occurred
			{
				recvMessage[Receive] = '\0';
			}
			else if (Receive == 0) // Connection was closed
			{
				cout << "Connection closed\n";
				closesocket(listenSocket);
				cleanup(acceptSocket);
				return 0;
			}
			else // An error has occurred
			{
				cerr << "ERROR: Failed to receive message\n";
				closesocket(listenSocket);
				cleanup(acceptSocket);
				return 1;
			}

			strncpy_s(machineID, recvMessage, sizeof(recvMessage));

			if (isNew)
			{
				ofstream outFile(filename, ofstream::app);
				outFile << serialNum;
				outFile << endl;
				outFile << machineID;
				outFile.close();

				int serial = send(acceptSocket, validStatus, strlen(validStatus), 0);

				if (serial == SOCKET_ERROR)
				{
					cerr << "ERROR: Failed to send serialNum\n";
					closesocket(listenSocket);
					cleanup(acceptSocket);
					return 1;
				}

				closesocket(acceptSocket);
			}
			else
			{
				int  check = 0;

				ifstream inFile(filename, ifstream::in);

				while (!inFile.eof() && !done)
				{
					if (check == 0)
					{
						inFile.getline(checkSerial, STRLEN);
						if (strcmp(checkSerial, serialNum) == 0)
						{
							foundSerial = true;
						}
						check++;
					}
					else if (check == 1)
					{
						inFile.getline(checkID, STRLEN);
						if (strcmp(checkID, machineID) == 0)
						{
							foundID = true;
						}
						check++;

						if (strcmp(checkSerial, serialNum) == 0 && strcmp(checkID, machineID) == 0)
						{
							matchFound = true;

							int serial = send(acceptSocket, validStatus, strlen(validStatus), 0);

							if (serial == SOCKET_ERROR)
							{
								cerr << "ERROR: Failed to send serialNum\n";
								closesocket(listenSocket);
								cleanup(acceptSocket);
								return 1;
							}
							closesocket(acceptSocket);
							done = true;
						}
					}
					else
					{
						check = 0;
					}
				}
				if (!matchFound && !foundSerial && !foundID)
				{
					ofstream outFile(filename, ofstream::app);
					outFile << serialNum;
					outFile << endl;
					outFile << machineID;
					outFile << endl;
					outFile.close();

					int serial = send(acceptSocket, validStatus, strlen(validStatus), 0);

					cout << "SerialNum: " << serialNum << endl;
					cout << "OK" << endl;
					cout << "MachineID: " << machineID << endl;
					cout << "OK" << endl;

					if (serial == SOCKET_ERROR)
					{
						cerr << "ERROR: Failed to send serialNum\n";
						closesocket(listenSocket);
						cleanup(acceptSocket);
						return 1;
					}

					closesocket(acceptSocket);
				}
				else if (matchFound && foundSerial && foundID)
				{
					cout << "MachineID: " << checkID << endl;
					cout << "OK" << endl;
					cout << "SerialNum: " << checkSerial << endl;
					cout << "OK" << endl;
				}
				else
				{
					cout << "SerialNum: " << serialNum << endl;
					cout << "OK" << endl;
					cout << "MachineID: " << machineID << endl;
					cout << "FAILED" << endl;

					cout << "\nAlready existed\n";

					int serial = send(acceptSocket, invalidStatus, strlen(invalidStatus), 0);

					if (serial == SOCKET_ERROR)
					{
						cerr << "ERROR: Failed to send serialNum\n";
						closesocket(listenSocket);
						cleanup(acceptSocket);
						return 1;
					}

					closesocket(acceptSocket);
				}
			}
		}
	}


	cleanup(listenSocket);

	return 0;
}

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}