#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

const int STRLEN = 256;

// Closes the socket and performs the WSACleanup
void cleanup(SOCKET socket);

int main()
{
	WSADATA		wsaData;
	SOCKET		listenSocket;
	SOCKET		acceptSocket;
	SOCKADDR_IN	serverAddr;
	int			port;
	char		sendMessage[STRLEN];
	char		recvMessage[STRLEN];
	bool		done = false;

	cout << "Enter port number to use: ";
	cin >> port;
	cin.ignore();

	// Loads Windows DLL (Winsock version 2.2) used in network programming
	if ( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
	{
		cerr << "ERROR: Problem with WSAStartup\n";
		return 1;
	}

	// Create a new socket to listen for client connections
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ( listenSocket == INVALID_SOCKET )
	{
		cerr << "ERROR: Cannot create socket\n";
		WSACleanup();
		return 1;
	}

	// Setup a SOCKADDR_IN structure which will be used to hold address
	// and port information. Notice that the port must be converted
	// from host byte order to network byte order.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );	
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	//serverAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );   <== deprecated

	// Attempt to bind to the port.
	if ( bind ( listenSocket, (SOCKADDR*) &serverAddr, sizeof(serverAddr) ) == SOCKET_ERROR )
	{
		cerr << "ERROR: Cannot bind to port\n";
		cleanup( listenSocket );		
		return 1;
	}

	// Start listening for incoming connections
	if ( listen ( listenSocket, 1 ) == SOCKET_ERROR )
    {
        cerr << "ERROR: Problem with listening on socket\n";
        cleanup( listenSocket );
        return 1;
    }

	cout << "\nWaiting for connections...\n";
	
	// Accept incoming connection.  Program pauses here until
	// a connection arrives.
	acceptSocket = accept( listenSocket, NULL, NULL );
	
	// For this program, the listen socket is no longer needed so it will be closed
	closesocket(listenSocket);

	cout << "Connected...\n\n";

	while (!done)
	{
		// Wait to receive a message from the remote computer
		cout << "\n\t--WAIT--\n\n";
		int iRecv = recv(acceptSocket, recvMessage, STRLEN, 0);
		if (iRecv > 0)
		{
			recvMessage[iRecv] = '\0';
			cout << "Recv > " << recvMessage << "\n";
		}
		else if (iRecv == 0)
		{
			cout << "Connection closed\n";
			cleanup( acceptSocket );
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			cleanup( acceptSocket );
			return 1;
		}

		// Communication ends if client sends an "end" message
		if (strcmp(recvMessage, "end") == 0) done = true;

		if (!done)
		{

			// Get a reply message from the user and attempt to send it to the remote computer
			cout << "Send > ";
			cin.get(sendMessage, STRLEN);
			cin.ignore();
			int iSend = send(acceptSocket, sendMessage, strlen(sendMessage), 0);
			if (iSend == SOCKET_ERROR)
			{
				cerr << "ERROR: Failed to send message\n";
				cleanup(acceptSocket);
				return 1;
			}
			
			// Communication ends if server sent an "end" message
			if (strcmp(sendMessage, "end") == 0) done = true;
		}
	}

	cleanup( acceptSocket );

	return 0;
}

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}

