
#ifndef CUDP_H
#define CUDP_H
#pragma warning(disable: 4668)
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

#define ONLY_DST	   0
#define ONLY_MY		   1
#define USING_BOTH     2

class  CUDP {
private:
	WSADATA wsaData;              /* Structure for WinSock setup communication */

	SOCKET sock;                  /* Socket */

	struct sockaddr_in DstAddr;   /* Destination */
	struct sockaddr_in MyAddr;    /* Local address */
	struct sockaddr_in tempAddr;

	int Dstlen;					  /* DstAddr length*/

	int sendDatalen;			  /* Send Data length*/
	int recvDatalen;			  /* Receive Data length*/

	DWORD recvTimeOut;

public:

	//socket_flag = 0 -> No bind() & Just use Destination sokcet
	//socket_flag = 1 -> Using bind(), Just make My socket
	//socket_flag = else -> Using the Dst & My socket
	bool UDP_init(int socket_flag, const char* socket_ip, int socket_port, char* Socket_name, const char* My_ip = NULL, int My_port = -1);  /* UDP initialize*/

	int Send(void* Data, int data_size); /* return send data length */
	int Recv(void* Data, int data_size); /* return recv data length */
	int Send_temp(char* Data, int data_size); /* return send data length */

	int Interprete_Client_Inform(char* Data);/*Interprete Client Information(IP,Port . . .)*/
	int Get_Socket_Info(char* Data);

	int CleanSocket();

	~CUDP();					  /* Clean up socket*/

};

#endif