#include "CUDP.h"


bool CUDP::UDP_init(int socket_flag, const char* socket_ip, int socket_port, char* Socket_name, const char* My_ip, int My_port) {  /* UDP initialize*/

	recvTimeOut = 10;//20msec

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		fprintf(stderr, "WSAStartup() failed");
		exit(1);
	}

	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {//IPPROTO_UDP
		fprintf(stderr, "socket() failed");
		//exit(1);
		return false;
	}
	
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeOut, sizeof(recvTimeOut));

	if ((socket_flag == 0) || (socket_flag == 2)) {
		memset(&DstAddr, 0, sizeof(DstAddr));
		DstAddr.sin_family = AF_INET;
		DstAddr.sin_addr.s_addr = inet_addr(socket_ip);
		DstAddr.sin_port = htons(socket_port);
	}
	//shutdown(sock, SHU);
	if ((socket_flag == 1) || (socket_flag == 2)) {

		memset(&MyAddr, 0, sizeof(MyAddr));
		MyAddr.sin_family = AF_INET;
		//MyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		MyAddr.sin_addr.s_addr = inet_addr(My_ip);
		MyAddr.sin_port = htons(My_port);

		if (bind(sock, (SOCKADDR *)&MyAddr, sizeof(MyAddr)) != 0) {
			printf("bind() err\n");
			return false;
		}

	}

	printf("Success %s socket init !!!\n", Socket_name);
	return true;
}

int CUDP::Send(void* Data, int data_size) {

	if ((sendDatalen = sendto(sock, (char *)Data, data_size, 0, (struct sockaddr *) &DstAddr,
		sizeof(DstAddr))) != data_size) {
		printf("send err, send Data size : %d\n", sendDatalen);
	}
	return sendDatalen;
}

int CUDP::Send_temp(char* Data, int data_size) {

	if ((sendDatalen = sendto(sock, Data, data_size, 0, (struct sockaddr *) &tempAddr,
		sizeof(tempAddr))) != data_size) {
		printf("send err, send Data size : %d\n", sendDatalen);
	}
	return sendDatalen;
}

int CUDP::Recv(void* Data, int data_size) {

	Dstlen = sizeof(DstAddr);
	memset(Data, 0, data_size);
	/* Block until receive message from a client */
	if ((recvDatalen = recvfrom(sock, (char *)Data, data_size, 0,
		(struct sockaddr *) &DstAddr, &Dstlen)) < 0) {
		//printf("Recv Err\n");
		return -1;
	}
	return recvDatalen;
}

int CUDP::Interprete_Client_Inform(char* Data) {

	memset(&tempAddr, 0, sizeof(tempAddr));
	memcpy(&tempAddr, Data, sizeof(tempAddr));

	printf("Client Port : %d, IP : %s\n", ntohs(tempAddr.sin_port), inet_ntoa(tempAddr.sin_addr));
	printf("Copy to DstAddr!\n");

	return TRUE;
}

int CUDP::Get_Socket_Info(char* Data) {

	memcpy(Data, &DstAddr, sizeof(DstAddr));
	printf("Port %d, IP : %s\n\n", ntohs(DstAddr.sin_port), inet_ntoa(DstAddr.sin_addr));
	return TRUE;
}

int CUDP::CleanSocket() {

	if (sock != NULL) {
		closesocket(sock);
		sock = NULL;
	}
	return 0;
}

CUDP::~CUDP() {
	if (sock != NULL) {
		closesocket(sock);
		sock = NULL;
	}
	/*WSACleanup();  *//* Cleanup Winsock */
					   //exit(1);
}