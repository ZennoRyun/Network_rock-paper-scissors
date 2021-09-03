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

	sock = socket(PF_INET, SOCK_STREAM, 0);     //�������ϻ���
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
			fputs("���������� �Է��Ͻÿ�(���ڷ�) : ", stdout);
			fgets(msg, BUF_SIZE, stdin);
			if (!strcmp(msg, "q\n")) break;
			else if (!strcmp(msg, "1\n")) break;
			else if (!strcmp(msg, "2\n")) break;
			else if (!strcmp(msg, "3\n")) break;
			else {
				gotoxy(RIGHT - 11, 18);
				fputs("�߸��� �Է��Դϴ�.", stdout);
			}
		}
		if (!strcmp(msg, "q\n"))
		{
			clrscr();
			break;
		}

		board();

		RSP(msg[0], 1);                                                                        //Ŭ���̾�Ʈ ȭ�鿡 ǥ��

		strl = send(sock, msg, strlen(msg), 0);                                             //������ msg ������ ����

		if (strl == SOCKET_ERROR)
			ErroHandling("send() error");

		strl = recv(sock, &rnum, 1, 0);                                                            //����� ��� ����
		if (strl == SOCKET_ERROR)
			ErroHandling("recv() error");

		if (rnum == 4 || rnum == 5)                                                              // int�� ������� ������ �̱���
		{
			gotoxy(LEFT - 4, ST - 2);                                                              // �ڽ��� ȭ�鿡 ���
			fputs("�¸� !!", stdout);
			val = (msg[0] + 1) % 3 + 1;                                                            // ����� �Է°� ���
			RSP(val, 2);                                                                                                //����� ȭ�鿡 �Է°� ���
			money += BAT;                                                                                            // �Ӵ�����
			gotoxy(18, 22); printf("%d ��   ", money);                                                                  //�Ӵ� ���
			wCount += 1;
			gotoxy(18, 25); printf("%d��, %d��, %d��", wCount, dCount, lCount);                                        //�Ӵ� ���
		}
		else if (rnum <= '3' && rnum >= '1')                                                                  // char�� �Է°��� ������ �����
		{
			gotoxy(RIGHT - 4, ST - 2);                                                                         // ����� ȭ�鿡 ���
			fputs("�¸� !!", stdout);
			RSP(rnum, 2);                                                                                //���� ����� �Է°� ���
			money -= BAT;                                                                                        //�Ӵ� ����
			gotoxy(18, 22); printf("%d ��   ", money);
			lCount += 1;
			gotoxy(18, 25); printf("%d��, %d��, %d��", wCount, dCount, lCount);                                                       //�Ӵ� ���
			if (money <= 0)     // �Ӵϰ� 0���ϰ� �Ǹ� ���� ����
			{
				gotoxy(LEFT - 4, ST - 2);
				fputs("GAMEOVER !!", stdout);
				break;
			}

		}
		else if (rnum == 6)                                                                                     // ���
		{
			gotoxy(LEFT - 4, ST - 2);                                                                            // ����� �Է°��� �޾Ƽ� ���ȭ�鿡 ���
			fputs("��� !!", stdout);
			gotoxy(RIGHT - 4, ST - 2);
			fputs("��� !!", stdout);
			RSP(msg[0], 2);
			dCount += 1;
			gotoxy(18, 25); printf("%d��, %d��, %d��", wCount, dCount, lCount);
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


void RSP(char a, int dir)       // ���������� ȭ�� ����Լ�
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

void board(void)                                                                                                     // ���� �ȳ� �� ��Ȳ �� ���� �Լ� 
{
	clrscr();
	setcursortype(NOCURSOR);

	gotoxy(1, 2); printf("             ��                                     ���");
	gotoxy(1, 17); printf("    # ������ %d�� #", BAT);
	gotoxy(1, 21); printf("                                                   �� �� : 1");
	gotoxy(1, 22); printf("    USER MONEY :                                   �� �� : 2");
	gotoxy(1, 23); printf("                                                     ��  : 3");
	gotoxy(1, 25); printf("    ���� ���� :                                     ���� : q");
	gotoxy(18, 22); printf("%d ��", money);
}

void ErroHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}