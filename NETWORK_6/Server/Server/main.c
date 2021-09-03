#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024
void ErrorHandling(char* msg);      //�����ڵ鸵 �Լ�
char RSP(char a, char b);       //���������� �Լ�

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET servSock;
	SOCKET clntSock;
	SOCKADDR_IN servAdr;
	SOCKADDR_IN clntAdr;

	int clntAdrSz;
	int strl;
	int handleIndex;
	int adr1, adr2;

	char msg[BUF_SIZE] = { 0, };

	fd_set reads, temp;
	TIMEVAL timeout;

	char user1 = 0, user2 = 0, result = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	servSock = socket(PF_INET, SOCK_STREAM, 0);     //�������ϻ���
	if (servSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAdr, 0, sizeof(servAdr));       //���� ���� �Է�
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(5000);

	if (bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	FD_ZERO(&reads);                                                                                                // fd_set �ʱ�ȭ
	FD_SET(servSock, &reads);                                                                              // �������� ���� ����

	while (1)
	{
		temp = reads;
		timeout.tv_sec = 5;                                                                                       // �����ð� ����
		timeout.tv_usec = 0;

		if (select(0, &temp, 0, 0, &timeout) == SOCKET_ERROR)                                           // ����Ʈȣ��
			ErrorHandling("select() error");

		for (handleIndex = 0; handleIndex < reads.fd_count; handleIndex++)                               // ��ȭ�ִ� �ڵ� ����
		{
			if (FD_ISSET(reads.fd_array[handleIndex], &temp))
			{
				if (reads.fd_array[handleIndex] == servSock)
				{
					clntAdrSz = sizeof(clntAdr);
					clntSock = accept(servSock, (SOCKADDR*)&clntAdr, &clntAdrSz);
					if (clntSock == INVALID_SOCKET)
						ErrorHandling("accept() error");

					FD_SET(clntSock, &reads);                                                                            // Ŭ���̾�Ʈ���ϰ��ü���     
				}
				else                                                                                                          // Ŭ���̾�Ʈ�����ΰ��(�����������̰ų� ����)
				{
					strl = recv(reads.fd_array[handleIndex], msg, BUF_SIZE - 1, 0);                               // ��Ŷ�����͸� ����
					if (strl == SOCKET_ERROR)
						ErrorHandling("recv() error");

					else if (strl == 0)                                                                               // �����û�� ���
					{
						closesocket(temp.fd_array[handleIndex]);                                         // �����ûŬ���̾�Ʈ ���ϴ���
						FD_CLR(reads.fd_array[handleIndex], &reads);                                          // �����ڵ� ���˻���
					}
					else                                                                                                      // ������ ������ ���
					{
						if (user1 == 0)                                                                             // ������ �ƹ��� ������
						{
							user1 = msg[0];
							adr1 = reads.fd_array[handleIndex];
						}
						else                                                                                              // ������ �� ���� ��
						{
							user2 = msg[0];
							adr2 = reads.fd_array[handleIndex];
						}

						if (user1 != 0 && user2 != 0)                                                 // �Է��� �Ϸ�Ǹ� ���������� ����
						{
							result = RSP(user1, user2);
							if (result == 4)  printf("���� ��� : user1 ��\n");
							if (result == 5)  printf("���� ��� : user2 ��\n");
							else printf("���� ��� : ���º�\n");
						}

						if (result != 0)                                                                                                        // ����� ���� ��
						{
							if (result == 4)                                                                                               // ���� �Է��� user1�� �̱��
							{
								send(adr1, &result, 1, 0);                                                                                        // user1���� ����� ����
								printf("user1 %d ������ �� :%d\n", adr1, result);
								send(adr2, &user1, 1, 0);                                                                                                      // user2���� user1�� �Է°� ����
								printf("user2 %d ������ �� :%c\n", adr2, user1);
							}
							else if (result == 5)                                                                              // ���߿� �Է��� user2�� �̱��
							{
								send(adr2, &result, 1, 0);                                                              // user2���� ����� ����
								printf("user2 %d ������ �� :%d\n", adr2, result);
								send(adr1, &user2, 1, 0);                                                                            // user1���� user2�� �Է°� ����
								printf("user1 %d ������ �� :%c\n", adr1, user2);
							}
							else                                                                                                  // ��� ��
							{
								send(adr1, &result, 1, 0);                                                                  //  ���ο��� ���� �� ����
								printf("user1 %d ������ �� :%d\n", adr1, result);
								send(adr2, &result, 1, 0);
								printf("user2 %d ������ �� :%d\n", adr2, result);
							}
							user1 = 0;
							user2 = 0;
							result = 0;
						}

					}
				}
			}
		}
	}
	WSACleanup();

	return 0;
}

char RSP(char a, char b)
{
	if (a == b)
		return 6;       //����� ��
	else if (((int)a) % 3 == ((int)b + 1) % 3)
		return 4;       //user1 �¸�
	else
		return 5;       //user2 �¸� 
}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}