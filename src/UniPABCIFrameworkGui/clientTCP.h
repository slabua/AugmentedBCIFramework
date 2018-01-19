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

#ifndef CLIENTTCP_H
#define CLIENTTCP_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <string>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"
#define DEFAULT_ADDRESS "127.0.0.2"


class clientTCP
{
public:
    clientTCP();
	clientTCP(std::string,std::string);
    void sendCommand(const char* s);
	void sendCommandToRobot(const char* s);
protected:
	WSADATA wsaData;
    SOCKET ConnectSocket;
    struct addrinfo *result,
                    *ptr,
                    hints;
    const char *sendbuf;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen;
	std::string port;
	std::string address;
};

#endif // CLIENTTCP_H
