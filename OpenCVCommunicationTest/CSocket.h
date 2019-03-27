
#ifndef _CSOCKET_H
#define	_CSOCKET_H

#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

#define SOCKET_SUCCESS 0
#define SOCKET_SD_FAIL 1//Send			fail
#define SOCKET_RV_FAIL 2//Receive		fail
#define SOCKET_BI_FAIL 3//bind			fail
#define SOCKET_SO_FAIL 4//Socket create fail
#define SOCKET_AC_FAIL 5//Accept		fail
#define SOCKET_LI_FAIL 6//Listen		fail

#define PRIORITY_CLIENT 1//Listen		fail
#define PRIORITY_SERVER 2//Listen		fail

#define HEAD 0xf000
#define TAIL 0x000f

//Server//
class CServer{

private:
	int serv_sock;
	int clnt_sock;
	int clnt_addr_size;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	int Port_Num;

	
public:

	CServer();
	~CServer();
	int Server_init(int Port);
	int Send_Data(char *Send_Data,int Data_Size);
	int Receive_Data(char *Recv_Data,int Data_Size);
	
};

//Client//
class CClient{
private:
	
	SOCKET cliSocket;
	SOCKADDR_IN servAddr;


public:
	CClient();
	~CClient();
	int Client_init(int Port,char *IP);
	int Send_Data(char *Send_Data,int Data_Size);
	int Receive_Data(char *Recv_Data,int Data_Size, int& _received_data);
};


#endif