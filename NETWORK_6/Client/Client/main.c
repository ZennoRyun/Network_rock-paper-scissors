#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>

#define clrscr() system("cls")

#define NOCURSOR  0
#define SOLIDCURSOR  1
#define NORMALCURSOR 2

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define GAMEMONEY 10000
#define ST 5
#define BAT 2000
#define LEFT 16
#define RIGHT 55

void gotoxy(int x, int y);
void setcursortype(int c);
void RSP(char a, int dir);
void board(void);

void ErroHandling(char* msg);

int money = GAMEMONEY;
int wCount = 0, dCount = 0, lCount = 0;
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET sock;

	SOCKADDR_IN servAddr;


	int strl;
	char rnum, val, enter;

	char msg[BUF_SIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErroHandling("WSAStartup() error!");

	sock = socket(PF_INET, SOCK_STREAM, 0);     //서버소켓생성
	if (sock == INVALID_SOCKET)
		ErroHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(5000);

	if (connect(sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErroHandling("connect() error");

	board();

	while (1)
	{
		while (1) {
			gotoxy(RIGHT - 11, 17);
			fputs("가위바위보 입력하시오(숫자로) : ", stdout);
			fgets(msg, BUF_SIZE, stdin);
			if (!strcmp(msg, "q\n")) break;
			else if (!strcmp(msg, "1\n")) break;
			else if (!strcmp(msg, "2\n")) break;
			else if (!strcmp(msg, "3\n")) break;
			else {
				gotoxy(RIGHT - 11, 18);
				fputs("잘못된 입력입니다.", stdout);
			}
		}
		if (!strcmp(msg, "q\n"))
		{
			clrscr();
			break;
		}

		board();

		RSP(msg[0], 1);                                                                        //클라이언트 화면에 표시

		strl = send(sock, msg, strlen(msg), 0);                                             //서버로 msg 데이터 전송

		if (strl == SOCKET_ERROR)
			ErroHandling("send() error");

		strl = recv(sock, &rnum, 1, 0);                                                            //연산된 결과 수신
		if (strl == SOCKET_ERROR)
			ErroHandling("recv() error");

		if (rnum == 4 || rnum == 5)                                                              // int형 결과값을 받으면 이긴사람
		{
			gotoxy(LEFT - 4, ST - 2);                                                              // 자신의 화면에 출력
			fputs("승리 !!", stdout);
			val = (msg[0] + 1) % 3 + 1;                                                            // 상대편 입력값 계산
			RSP(val, 2);                                                                                                //상대편 화면에 입력값 출력
			money += BAT;                                                                                            // 머니증가
			gotoxy(18, 22); printf("%d 원   ", money);                                                                  //머니 출력
			wCount += 1;
			gotoxy(18, 25); printf("%d승, %d무, %d패", wCount, dCount, lCount);                                        //머니 출력
		}
		else if (rnum <= '3' && rnum >= '1')                                                                  // char형 입력값을 받으면 진사람
		{
			gotoxy(RIGHT - 4, ST - 2);                                                                         // 상대의 화면에 출력
			fputs("승리 !!", stdout);
			RSP(rnum, 2);                                                                                //받은 상대편 입력값 출력
			money -= BAT;                                                                                        //머니 감소
			gotoxy(18, 22); printf("%d 원   ", money);
			lCount += 1;
			gotoxy(18, 25); printf("%d승, %d무, %d패", wCount, dCount, lCount);                                                       //머니 출력
			if (money <= 0)     // 머니가 0이하가 되면 게임 종료
			{
				gotoxy(LEFT - 4, ST - 2);
				fputs("GAMEOVER !!", stdout);
				break;
			}

		}
		else if (rnum == 6)                                                                                     // 비김
		{
			gotoxy(LEFT - 4, ST - 2);                                                                            // 상대의 입력값을 받아서 상대화면에 출력
			fputs("비김 !!", stdout);
			gotoxy(RIGHT - 4, ST - 2);
			fputs("비김 !!", stdout);
			RSP(msg[0], 2);
			dCount += 1;
			gotoxy(18, 25); printf("%d승, %d무, %d패", wCount, dCount, lCount);
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}

void gotoxy(int x, int y)
{

	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void setcursortype(int c)
{
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}


void RSP(char a, int dir)       // 가위바위보 화면 출력함수
{
	int line;
	if (a == '1' || a == 1)
	{
		if (dir == 1)
			line = 2;
		else if (dir == 2)
			line = 40;


		gotoxy(line, ST + 1); printf("  @@@@@@@@  @@         @@@@@   @@  ");
		gotoxy(line, ST + 2); printf("        @@  @@        @     @  @@  ");
		gotoxy(line, ST + 3); printf("       @@   @@@@       @@@@@   @@  ");
		gotoxy(line, ST + 4); printf("      @@    @@                 @@  ");
		gotoxy(line, ST + 5); printf("     @@     @@      @@@@@@@@@@@@@  ");
		gotoxy(line, ST + 6); printf("    @@      @@          @@@    @@  ");
		gotoxy(line, ST + 7); printf("   @@       @@          @@@    @@  ");
		gotoxy(line, ST + 8); printf("            @@          @@@    @@  ");
	}
	else if (a == '2' || a == 2)
	{
		if (dir == 1)
			line = 2;
		else if (dir == 2)
			line = 40;
		gotoxy(line, ST + 1); printf("  @@    @@  @@         @@@@@   @@  ");
		gotoxy(line, ST + 2); printf("  @@    @@  @@        @     @  @@  ");
		gotoxy(line, ST + 3); printf("  @@    @@  @@@@       @@@@@   @@  ");
		gotoxy(line, ST + 4); printf("  @@@@@@@@  @@                 @@  ");
		gotoxy(line, ST + 5); printf("  @@    @@  @@      @@@@@@@@@@@@@  ");
		gotoxy(line, ST + 6); printf("  @@    @@  @@          @@@    @@  ");
		gotoxy(line, ST + 7); printf("  @@@@@@@@  @@          @@@    @@  ");
		gotoxy(line, ST + 8); printf("            @@          @@@    @@  ");
	}
	else if (a == '3' || a == 3)
	{
		if (dir == 1)
			line = 2;
		else if (dir == 2)
			line = 40;

		gotoxy(line, ST); printf("                                   ");
		gotoxy(line, ST + 1); printf("          @@@         @@@          ");
		gotoxy(line, ST + 2); printf("          @@@         @@@          ");
		gotoxy(line, ST + 3); printf("          @@@@@@@@@@@@@@@          ");
		gotoxy(line, ST + 4); printf("          @@@         @@@          ");
		gotoxy(line, ST + 5); printf("          @@@@@@@@@@@@@@@          ");
		gotoxy(line, ST + 6); printf("                @@@                ");
		gotoxy(line, ST + 7); printf("                @@@                ");
		gotoxy(line, ST + 8); printf("      @@@@@@@@@@@@@@@@@@@@@@@      ");
		gotoxy(line, ST + 9); printf("                                   ");
	}
}

void board(void)                                                                                                     // 게임 안내 및 상황 판 구현 함수 
{
	clrscr();
	setcursortype(NOCURSOR);

	gotoxy(1, 2); printf("             나                                     상대");
	gotoxy(1, 17); printf("    # 배팅은 %d원 #", BAT);
	gotoxy(1, 21); printf("                                                   가 위 : 1");
	gotoxy(1, 22); printf("    USER MONEY :                                   바 위 : 2");
	gotoxy(1, 23); printf("                                                     보  : 3");
	gotoxy(1, 25); printf("    나의 전적 :                                     종료 : q");
	gotoxy(18, 22); printf("%d 원", money);
}

void ErroHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}