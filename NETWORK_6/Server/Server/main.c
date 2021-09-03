#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024
void ErrorHandling(char* msg);      //에러핸들링 함수
char RSP(char a, char b);       //가위바위보 함수

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

	servSock = socket(PF_INET, SOCK_STREAM, 0);     //서버소켓생성
	if (servSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAdr, 0, sizeof(servAdr));       //서버 정보 입력
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(5000);

	if (bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	FD_ZERO(&reads);                                                                                                // fd_set 초기화
	FD_SET(servSock, &reads);                                                                              // 서버소켓 감시 설정

	while (1)
	{
		temp = reads;
		timeout.tv_sec = 5;                                                                                       // 지연시간 설정
		timeout.tv_usec = 0;

		if (select(0, &temp, 0, 0, &timeout) == SOCKET_ERROR)                                           // 셀렉트호출
			ErrorHandling("select() error");

		for (handleIndex = 0; handleIndex < reads.fd_count; handleIndex++)                               // 변화있는 핸들 감지
		{
			if (FD_ISSET(reads.fd_array[handleIndex], &temp))
			{
				if (reads.fd_array[handleIndex] == servSock)
				{
					clntAdrSz = sizeof(clntAdr);
					clntSock = accept(servSock, (SOCKADDR*)&clntAdr, &clntAdrSz);
					if (clntSock == INVALID_SOCKET)
						ErrorHandling("accept() error");

					FD_SET(clntSock, &reads);                                                                            // 클라이언트소켓감시설정     
				}
				else                                                                                                          // 클라이언트소켓인경우(데이터전송이거나 종료)
				{
					strl = recv(reads.fd_array[handleIndex], msg, BUF_SIZE - 1, 0);                               // 패킷데이터를 수신
					if (strl == SOCKET_ERROR)
						ErrorHandling("recv() error");

					else if (strl == 0)                                                                               // 종료요청인 경우
					{
						closesocket(temp.fd_array[handleIndex]);                                         // 종료요청클라이언트 소켓닫음
						FD_CLR(reads.fd_array[handleIndex], &reads);                                          // 종료핸들 점검삭제
					}
					else                                                                                                      // 데이터 수신인 경우
					{
						if (user1 == 0)                                                                             // 유저가 아무도 없을떄
						{
							user1 = msg[0];
							adr1 = reads.fd_array[handleIndex];
						}
						else                                                                                              // 유저가 한 명일 때
						{
							user2 = msg[0];
							adr2 = reads.fd_array[handleIndex];
						}

						if (user1 != 0 && user2 != 0)                                                 // 입력이 완료되면 가위바위보 진행
						{
							result = RSP(user1, user2);
							if (result == 4)  printf("게임 결과 : user1 승\n");
							if (result == 5)  printf("게임 결과 : user2 승\n");
							else printf("게임 결과 : 무승부\n");
						}

						if (result != 0)                                                                                                        // 결과가 나올 시
						{
							if (result == 4)                                                                                               // 먼저 입력한 user1이 이기면
							{
								send(adr1, &result, 1, 0);                                                                                        // user1에게 결과를 전송
								printf("user1 %d 전송한 값 :%d\n", adr1, result);
								send(adr2, &user1, 1, 0);                                                                                                      // user2에게 user1의 입력값 전송
								printf("user2 %d 전송한 값 :%c\n", adr2, user1);
							}
							else if (result == 5)                                                                              // 나중에 입력한 user2가 이기면
							{
								send(adr2, &result, 1, 0);                                                              // user2에게 결과를 전송
								printf("user2 %d 전송한 값 :%d\n", adr2, result);
								send(adr1, &user2, 1, 0);                                                                            // user1에게 user2의 입력값 전송
								printf("user1 %d 전송한 값 :%c\n", adr1, user2);
							}
							else                                                                                                  // 비길 때
							{
								send(adr1, &result, 1, 0);                                                                  //  서로에게 상대방 값 전송
								printf("user1 %d 전송한 값 :%d\n", adr1, result);
								send(adr2, &result, 1, 0);
								printf("user2 %d 전송한 값 :%d\n", adr2, result);
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
		return 6;       //비겼을 때
	else if (((int)a) % 3 == ((int)b + 1) % 3)
		return 4;       //user1 승리
	else
		return 5;       //user2 승리 
}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}