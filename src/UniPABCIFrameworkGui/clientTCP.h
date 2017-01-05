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
