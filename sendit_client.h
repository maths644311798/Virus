#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 23333

int initial_build() 
{
	//��ʼ��
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) 
	{
		return 1;
	}
	return 0;
}

SOCKET client_build() 
{
	if (initial_build()) 
	{
		perror("startup");
		return -1;
	}
	//�ͻ�***********************************************************************************************	
	//�ͻ���ֻ��Ҫһ���׽����ļ������������ںͷ�����ͨ��
	SOCKET clientSocket;
	//������������socket
	struct sockaddr_in serverAddr;

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	{
		perror("socket");
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	//ָ���������˵�ip�����ز��ԣ�127.0.0.1
	inet_pton(AF_INET, "127.0.0.1", (void*)&serverAddr.sin_addr);
	if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
	{
		perror("connect");
		return -1;
	}

	return clientSocket;
}

bool sendit(std::string key,std::string iv) 
{
	SOCKET clientSocket = client_build();
	if (clientSocket == SOCKET_ERROR) return false;

	char sendbuf[200];
	char recvbuf[200];
	int nNetTimeout = 1000;
	setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
	//��ʼ����**************************************************************************
	size_t i;
	for (i = 0;i < key.length();i++) 
	{
		sendbuf[i] = key[i];//���Ƶ�sendbuf
	}
	sendbuf[key.length()] = '\0';

	send(clientSocket, sendbuf, key.length(), 0);

	recv(clientSocket, recvbuf, 128, 0);
	for (i = 0;i < iv.length();i++) 
	{
		sendbuf[i] = iv[i];//���Ƶ�sendbuf
	}
	sendbuf[iv.length()] = '\0';

	send(clientSocket, sendbuf,iv.length(), 0);

	closesocket(clientSocket);
	return true;
}
