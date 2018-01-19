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
* Based on a previous work -P300 core- of (in alphabetical order):
* Rosario Misuraca, Walter Tranchina, Giuseppe Trubia.
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

#include "InterfaceListener.h"
#include <iostream>
//#include "ringbuffer.h"
#include <afxwin.h>

using std::cout;
using std::endl;


bool InterfaceListener::is_listening;
//bool InterfaceListener::valueReady;
bool InterfaceListener::_startedOnlineP300Interface;
int InterfaceListener::symbolTag;
CEvent InterfaceListener::_switchSimbolTag;
CEvent InterfaceListener::_startedCalibration;
CEvent InterfaceListener::_callNextStimulus;
CEvent InterfaceListener::_finishCalibration;
bool InterfaceListener::endedStimulus;


InterfaceListener::InterfaceListener() {

	is_listening = false;
	_startedOnlineP300Interface = false;
	endedStimulus = false;
}

void InterfaceListener::startAcquisition() {

	is_listening = true;

	//_networkTagThread = AfxBeginThread(listenNetwork, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
	//_networkTagThread->ResumeThread();
}

int InterfaceListener::bufferToInt(char* buffer) {

	return atoi(buffer);
}

CEvent& InterfaceListener::getSymbolTagEvent() {

	return _switchSimbolTag;
}

void InterfaceListener::setEndedStimulus() {

	endedStimulus = !endedStimulus;
}

bool InterfaceListener::getEndedStimulus() {

	return endedStimulus;
}

UINT InterfaceListener::ListenNetwork(LPVOID param) {

	WSADATA wsa;
	SOCKET master, new_socket, client_socket[1], s;
	struct sockaddr_in server, address;
	int max_clients = 1, activity, addrlen, i, valread;
	char *message = "";

	//size of our receive buffer, this is string length.
	int MAXRECV = 1024;
	//set of socket descriptors
	fd_set readfds;
	//1 extra for null character, string termination
	char *buffer;
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	for (int i=0; i<1; i++) {
		client_socket[i] = 0;
	}

	printf("InterfaceListener: Initialising Winsock... "); // SLB
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

		printf("InterfaceListener: Socket created.\n");

		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(8888);

		//std::bind(master, (struct sockaddr *)&server, sizeof(server));

		//Bind
		if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		puts("InterfaceListener: Bind done.\n"); 

		//Listen to incoming connections
		listen(master, 3);

		//Accept and incoming connection
		//  puts("Waiting for incoming connections...");

		addrlen = sizeof(struct sockaddr_in);

		while (is_listening == true) {
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
						//cout << "valore tag :" << value << endl;
						if (value == START_STIMULUS) {	//start tag from online interface
							cout << "Code ID: 200" << endl;
							_callNextStimulus.SetEvent();
						}
						else if (value == END_STIMULUS)	{ // end of stimulus and start of matlab elaboration
							//printf("E' arrivato il fine stimolo.\n");
							cout << "Code ID: -200" << endl;
							endedStimulus = true;
						}
						else if (value == START_CALIBRATION) {	//inizio calibrazione
							cout << "Code ID: 100" << endl;
							_startedCalibration.SetEvent();
						}
						else if (value == END_CALIBRATION) {	//fine calibrazione
							cout << "Code ID: -100" << endl;
							_finishCalibration.SetEvent();
						}
						else if (value >= 1 && value <= 36) {	//stimuli tag from interface
							_switchSimbolTag.SetEvent();		//signal the target changing
							symbolTag = value;
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

int InterfaceListener::getSymbolTag() {

	return symbolTag;
}

void InterfaceListener::stopAcquisition() {

	is_listening = false;
}

bool InterfaceListener::isStartedInterface() {

	return _startedOnlineP300Interface;
}

CEvent& InterfaceListener::getStartedCalibrationEvent() {

	return _startedCalibration;
}

CEvent& InterfaceListener::getCallNextStimulus() {

	return _callNextStimulus;
}
void InterfaceListener::setStartedInterface(bool b) {

	_startedOnlineP300Interface = b;
}

CEvent& InterfaceListener::getFinishCalibrationEvent() {

	return _finishCalibration;
}

/*
void InterfaceListener::sendtoNetwork(char* message) {
	try{
		//send the message
		if (sendto(s, message, strlen(message), 0,
			(struct sockaddr *) &si_other, slen) == SOCKET_ERROR && is_listening == TRUE) {
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
	}
	catch (const std::exception& ex) {
		printf("** RuntimeException from thread");
	}
}

unsigned int InterfaceListener::udpSocketInitialization() {
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(8888);
	si_other.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
}


void udpSocketTermination() {
	closesocket(s);
	WSACleanup();
}
*/