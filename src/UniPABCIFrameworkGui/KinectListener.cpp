/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*****************************************************************************/

#include "KinectListener.h"
#include <ws2tcpip.h> // for inet_pton
#include <iostream>
#include <afxwin.h>

using std::cout;
using std::endl;


bool KinectListener::isListening;
bool KinectListener::_startedKinect;
int KinectListener::gameMove;
CEvent KinectListener::_moveReceived;
CEvent KinectListener::_isRock;
CEvent KinectListener::_isPaper;
CEvent KinectListener::_isScissors;


KinectListener::KinectListener() {

	isListening	= false;
	_startedKinect	= false;
}

void KinectListener::startAcquisition() {

	isListening = true;
	
}

void KinectListener::stopAcquisition() {

	isListening = false;
}

int KinectListener::bufferToInt(char* buffer) {

	return atoi(buffer);
}

UINT KinectListener::ListenNetwork(LPVOID param) {

	WSADATA wsa;
	SOCKET master, new_socket, client_socket[1], s;
	struct sockaddr_in server, address;
	int max_clients = 1, activity, addrlen, i, valread;
	char *message = "";

	//size of our receive buffer, this is string length.
	int MAXRECV = 1;
	//set of socket descriptors
	fd_set readfds;
	//1 extra for null character, string termination
	char *buffer;
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	for (int i=0; i<1; i++) {
		client_socket[i] = 0;
	}

	printf("KinectListener: Initialising Winsock... "); // SLB
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");
	__try{
		//Create a socket
		if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			printf("Could not create socket : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		printf("KinectListener: Socket created.\n");

		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY; // SLB binds to any address
		//inet_pton(AF_INET, "127.0.0.3", &(server.sin_addr)); // SLB other method to bind to a particular address
		server.sin_port = htons(11000);

		//std::bind(master, (struct sockaddr *)&server, sizeof(server));

		//Bind
		if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		puts("KinectListener: Bind done.\n"); 

		//Listen to incoming connections
		listen(master, 3);

		//Accept and incoming connection
		//  puts("Waiting for incoming connections...");

		addrlen = sizeof(struct sockaddr_in);

		while (isListening) {
			//clear the socket fd set
			FD_ZERO(&readfds);

			//add master socket to fd set
			FD_SET(master, &readfds);

			//add child sockets to fd set
			for (int i=0; i<max_clients; i++) {
				s = client_socket[i];
				if (s > 0) {
					FD_SET(s, &readfds);
				}
			}

			//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
			activity = select(0, &readfds, NULL, NULL, NULL);

			if (activity == SOCKET_ERROR) {
				printf("select call failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//If something happened on the master socket , then its an incoming connection
			if (FD_ISSET(master, &readfds)) {
				if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0) {
					perror("accept");
					exit(EXIT_FAILURE);
				}

				//add new socket to array of sockets
				for (int i=0; i<max_clients; i++) {
					if (client_socket[i] == 0) {
						client_socket[i] = new_socket;
						break;
					}
				}
			}

			//else its some IO operation on some other socket :)
			for (int i=0; i<max_clients; i++) {
				s = client_socket[i];
				//if client presend in read sockets             
				if (FD_ISSET(s, &readfds)) {
					//get details of the client
					getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

					//Check if it was for closing , and also read the incoming message
					//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
					valread = recv(s, buffer, MAXRECV, 0);

					if (valread == SOCKET_ERROR) {
						int error_code = WSAGetLastError();
						if (error_code == WSAECONNRESET) {
							//Somebody disconnected , get his details and print
							printf("Host disconnected unexpectedly , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

							//Close the socket and mark as 0 in list for reuse
							closesocket(s);
							client_socket[i] = 0;
						}
						else {
							printf("recv failed with error code : %d", error_code);
						}
					}

					if (valread == 0) {
						//Close the socket and mark as 0 in list for reuse
						closesocket(s);
						client_socket[i] = 0;
					}
					else {
						//add null character, if you want to use with printf/puts or other string handling functions
						buffer[valread] = '\0';

						int value = bufferToInt(buffer);
						gameMove = value;

						cout << "Game: Player move ID: " << value << endl;
						
						if (value == GAMEK_ROCK) {
							cout << "Game: Hand gesture recognised as: ROCK" << endl;
							_moveReceived.SetEvent();
							_isRock.SetEvent();
						}
						else if (value == GAMEK_PAPER) {
							cout << "Game: Hand gesture recognised as: PAPER" << endl;
							_moveReceived.SetEvent();
							_isPaper.SetEvent();
						}
						else if (value == GAMEK_SCISSORS) {
							cout << "Game: Hand gesture recognised as: SCISSORS" << endl;
							_moveReceived.SetEvent();
							_isScissors.SetEvent();
						}
						else if (value == GAMEK_READY) {
							cout << "Game: Kinect ready." << endl;
							_moveReceived.SetEvent();
						}
						else if (value == GAMEK_DISCONNECTED) {
							cout << "Game: Kinect ready." << endl;
							_moveReceived.SetEvent();
						}
						else { // SLB TODO CHECK correct values, this and above, returned by the kinect thread
							cout << "Game: Hand gesture NOT recognised." << endl;
						}
					}
				}
			}
		}
	}
	__finally{
		printf("\n closing stream data\n");
		AfxEndThread(0xdead);
	}

	// SLB delete[]
	/*
	delete[] message;
	delete[] buffer;
	*/

	return 0xdead;
}

int KinectListener::getMove() {

	_moveReceived.ResetEvent();

	return gameMove;
}

bool KinectListener::isStartedKinect() {

	return _startedKinect;
}

void KinectListener::setStartedKinect(bool b) {

	_startedKinect = b;
}

CEvent& KinectListener::getMoveReceivedEvent() {

	return _moveReceived;
}

CEvent& KinectListener::getIsRockEvent() {

	return _isRock;
}

CEvent& KinectListener::getIsPaperEvent() {

	return _isPaper;
}

CEvent& KinectListener::getIsScissorsEvent() {

	return _isScissors;
}

