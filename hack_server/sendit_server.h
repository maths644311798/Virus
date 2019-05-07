#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <fstream>
#include <iostream>
#include "rsa_key_iv_server.h"

#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 23333

int initial_build() 
{
	//��ʼ��
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) {
		return 1;
	}
	return 0;
}

SOCKET server_build() {
	if (initial_build()) {
		perror("startup");
		return -1;
	}
	//������**************************************************************************
	SOCKET serverSocket;
	//���������׽���sockaddr_in�ṹ��������ֱ��ʾ�ͻ��˺ͷ�����
	struct sockaddr_in server_addr;

	//��socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	//��ʼ���������˵��׽��֣�����htons��htonl���˿ں͵�ַת�������ֽ���
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	//ip�����Ǳ���������ip��Ҳ�����ú�INADDR_ANY���棬����0.0.0.0���������е�ַ
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//bind
	if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect");
		return -1;
	}
	//���÷������ϵ�socketΪ����״̬
	if (listen(serverSocket, 5) < 0) {
		perror("listen");
		return -1;
	}
	printf("�����˿�: %d\n", SERVER_PORT);
	return serverSocket;
}

SOCKET get_client_in_server(SOCKET serverSocket) 
{
	//��client*******************************************************************************************
	struct sockaddr_in clientAddr;
	int addr_len = sizeof(clientAddr);
	SOCKET client;
	//����accept�����󣬻��������״̬��ֱ���յ���������
	client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
	if (client < 0) 
	{
		perror("accept");
		return -1;
	}
	printf("�ȴ���Ϣ...\n");

	//������
	char IP_dec[20];
	inet_ntop(AF_INET, (void *)&clientAddr.sin_addr, IP_dec, 16);
	printf("IP is %s\n", IP_dec);
	printf("Port is %d\n", htons(clientAddr.sin_port));

	return client;
}

int receive(SOCKET client_in_server,char str[]) {//-1ʧ��
	char buffer[200];
	int iDataNum;
	int nNetTimeout = 1000;

	if (client_in_server < 0) {
		perror("accept");
		return -1;
	}
	//����������***************************************************************************
	printf("��ȡ��Ϣ:");
	buffer[0] = '\0';

	setsockopt(client_in_server,SOL_SOCKET, SO_SNDTIMEO,(char *)&nNetTimeout, sizeof(int));
	setsockopt(client_in_server,SOL_SOCKET, SO_RCVTIMEO,(char *)&nNetTimeout, sizeof(int));
	iDataNum = recv(client_in_server, buffer, 200, 0);
	if (iDataNum < 0) 
	{
		perror("recv null");
		return -1;
	}
	buffer[iDataNum] = '\0';
	printf("%s\n", buffer);
	if (iDataNum > 0) 
	{
		for (int i = 0;i <= iDataNum;i++)
			str[i] = buffer[i];
		return iDataNum;
	}
	return -1;
}

void sendit_server() 
{
	//��key��iv******************
	
	SOCKET server = server_build();
	SOCKET client_in_server;
	char key[200];
	char iv[200];
	int key_len,iv_len;

	while (1) 
	{
		key[0] = '\0';
		iv[0] = '\0';
		key_len = -1;
		client_in_server = get_client_in_server(server);
		if (client_in_server != SOCKET_ERROR)
		{
			key_len = receive(client_in_server, key);
			send(client_in_server, "nothing\0", 7, 0);
			iv_len = receive(client_in_server, iv);
			closesocket(client_in_server);
			//д�ļ�***********
			std::string name = iv;
			std::ofstream out(name + ".txt", std::ios::out | std::ios::binary);
			const std::string private_path = "rsa-base64-private.key";
			std::string skey = key;
			key_decode_rsa(skey, private_path);
			out << skey;
			out.close();
		}
	}
}