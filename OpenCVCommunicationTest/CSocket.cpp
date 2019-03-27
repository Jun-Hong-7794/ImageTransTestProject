#include"CSocket.h"

//Server//
CServer::CServer(){
	
}

CServer::~CServer(){
	closesocket(clnt_sock);	
	WSACleanup();
}

int CServer::Server_init(int Port){

	char Send_Data[50] = "Connection with Server";
	WSADATA mywsadata;

	if(WSAStartup(0x0202,&mywsadata) != 0)
		printf("Server WSAStartup Err! \n");
	
	int option = 0;
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	option = 1;
	setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(char *)option,sizeof(option));


	if(serv_sock == -1){
		printf("socket() err\n");
		return SOCKET_SO_FAIL;
	}
	else
		printf("Create socket!\n");

	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serv_addr.sin_addr.s_addr = inet_addr(char*);
	serv_addr.sin_port = htons(Port);

	if( bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
		printf("bind() err\n");
		return SOCKET_BI_FAIL;
	}

	if(listen(serv_sock,5) == -1){
		printf("listen() err\n");
		return SOCKET_LI_FAIL;
	}
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);

	if(clnt_sock == -1){
		printf("accept() err\n");
		return SOCKET_AC_FAIL;
	}

	if(send(clnt_sock,Send_Data,sizeof(Send_Data),0) == -1){//Accept_Check
		printf("Accept_Check err\n");
		return SOCKET_SD_FAIL;
	}
	else
		printf("Connection With Masterstation Client. . .\n\n");

	return SOCKET_SUCCESS;
}

int CServer::Send_Data(char *Send_Data,int Data_Size){

//	float Temp[2];

	if(send(clnt_sock,Send_Data,sizeof(char)*Data_Size,0) == -1){//Accept_Check
		printf("Server_send err\n");
		return SOCKET_SD_FAIL;
	}
	return SOCKET_SUCCESS;
}

int CServer::Receive_Data(char *Recv_Data,int Data_Size){
	
	float Tmp[7];
	int recv_num = recv(clnt_sock,Recv_Data,sizeof(char)*Data_Size,0);
	if(recv_num == -1){
		printf("Server_recv err\n");
		return SOCKET_RV_FAIL;
	}
	return SOCKET_SUCCESS;
}

CClient::CClient(){

}

CClient::~CClient(){
	closesocket(cliSocket);
	WSACleanup();
}

int CClient::Client_init(int Client_Port,char *Client_IP){

	int scan_Num = 0;
	WSADATA mywsadata;

	if(WSAStartup(0x0202,&mywsadata) != 0)
		printf("Client WSAStartup Err! \n");

	char revMsg[50];

	printf("Press Enter Key to Connect with Robot Sever . . .\n");
	
	cliSocket = socket(PF_INET,SOCK_STREAM,0);
	memset(&servAddr,0,sizeof(servAddr));
	if(cliSocket == -1){
		printf("socket() err\n");
		return SOCKET_SO_FAIL;
	}
	else{
		printf("socket() OK\n");
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(Client_IP);
	servAddr.sin_port = htons(Client_Port);

	if(connect(cliSocket,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("connect() err\n");
		return SOCKET_SO_FAIL;
	}

	/*else{
		printf("connect() OK\n");
		if(recv(cliSocket,revMsg,sizeof(revMsg)-1,0) == -1){
			return SOCKET_RV_FAIL;
		}
		printf("Message From Linux Machine : %s\n\n",revMsg);
	}*/

	return SOCKET_SUCCESS;
}

int CClient::Send_Data(char *Send_Data,int Data_Size){//15

	if(send(cliSocket,Send_Data,sizeof(char)*Data_Size,0) == -1){//Accept_Check
		//printf("Client send err\n");
		int a = 0;
	}
		return SOCKET_SUCCESS;
}

int CClient::Receive_Data(char *Recv_Data,int Data_Size, int& _received_data){//685

	int receive_num = 0;
	int remain_Data_Number = 0;
	bool receive_num_err_Flag = false;
	float Tmp_Recv_Data[2] = {0,0};


	_received_data = recv(cliSocket, Recv_Data, sizeof(char)*Data_Size, 0);

	//printf("W : %f\n",Recv_Data[682]);
	//printf("V : %f\n\n",Recv_Data[683]);

	return SOCKET_SUCCESS;
}