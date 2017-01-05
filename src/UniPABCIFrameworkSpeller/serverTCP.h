#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


class serverTCP{

 public:
	 serverTCP();
	int receiveData(char* port);

 protected:
	WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket;
    SOCKET ClientSocket;

    struct addrinfo *result;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen;
};