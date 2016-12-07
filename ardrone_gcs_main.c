//ardrone 2.0 client programming by yang

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock.h>	//include windows socket API
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>

//Define keys
#define ESC			27
#define SPACEBAR	32
#define BACKSPACE	8
#define DEL			127
#define UP			72
#define DOWN		80
#define LEFT		75
#define	RIGHT		77

#pragma comment(lib,"Ws2_32.lib")

#define CTR_PORT		"5556"//control port(UDP)
#define IP		"192.168.1.1"

int main(int argc, char* argv[]){

	SOCKET	ctr_sockfd;

	WSADATA		ctr_wsaData;
	struct sockaddr_in ctr_server_addr, ctr_client_addr;

	char msg_buffer[1024] = { 0, };

	int seq = 0;
	int ctrRet;
	int ch = 0;
	int ch_config = 0;
	int takeoff = 0;

	float vx;	//roll
	float vy;	//pitch
	float vz;	//throttle
	float vr;	//yaw

	if ((WSAStartup(MAKEWORD(1, 1), &ctr_wsaData)) != NO_ERROR){
		printf("Winsock.dll Load Error\n");
		WSACleanup();
		return 1;
	}	//load winsock.dll

	ctr_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	//AF_INET:Internet, SOCK_DGRAM: UDP, IPPROTO_UDP: UDP, return socket descriptor SOCKET

	if (ctr_sockfd == INVALID_SOCKET){
		printf("ERROR: socket() failed. (%s, %d)\n", __FILE__, __LINE__);
		printf("소켓을 생성할수 없습니다.");
		closesocket(ctr_sockfd);
		WSACleanup();
		return 1;
	}

	// set server address
	memset((void *)&ctr_server_addr, 0x00, sizeof(ctr_server_addr));
	ctr_server_addr.sin_family = AF_INET;
	ctr_server_addr.sin_port = htons(atoi(CTR_PORT));
	ctr_server_addr.sin_addr.s_addr = inet_addr(IP);

	// Set the port and address of client
	memset(&ctr_client_addr, 0, sizeof(ctr_client_addr));
	ctr_client_addr.sin_family = AF_INET;
	ctr_client_addr.sin_port = htons(0);
	ctr_client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind the socket
	if (bind(ctr_sockfd, (SOCKADDR *)&ctr_client_addr, sizeof(ctr_client_addr)) == SOCKET_ERROR) {
		printf("ERROR: bind() failed. (%s, %d)\n", __FILE__, __LINE__);
		return 0;
	}


	for (seq = 0;;){

		//		system("cls");
		printf("setting\t\t[0]\n");
		if (takeoff == 0){
			printf("Takeoff\t\t[SPACE BAR]\n");
		}
		else{//takeoff == 1
			printf("Landing\t\t[SPACE BAR]\n");
		}
		printf("throttle up\t[UP KEY]\n");
		printf("throttle down\t[UP KEY]\n");
		printf("EMERGENCY\t[`]\n");
		printf("exit\t\t[ESC]\n");
		printf(">> ");

		ch = _getch();

		switch (ch){
		case '0':
			system("cls");
			printf("1. 수평 설정\n");
			printf("2. General Configuretion<SDK 8.4 참조>\n");
			printf("3. Control Configuretion<SDK 8.5 참조>\n");
			printf("4. Network Configuretion<SDK 8.6 참조>\n");
			printf("5. Nav-board Configuretion<SDK 8.7 참조>\n");
			printf("6 Video Configuretion<SDK 8.8 참조>\n");
			printf("7 Leds Configuretion<SDK 8.9 참조>\n");
			printf("8 Detection Configuretion<SDK 8.10 참조>\n");
			printf("9 GPS Configuretion<SDK 8.13 참조>\n");
			printf("백스페이스: 상위 메뉴로 돌아가기\n");
			printf(">> ");

			ch_config = _getch();

			switch (ch_config){
			case '1':
				sprintf(msg_buffer, "AT*FTRIM=%d\r", ++seq); //TRIM(수평 설정)
				break;
			case '2':case '3':case '4':case '5':case '6': case '7':case '8': case '9':
				printf("구현 예정\n");
				break;
			case BACKSPACE:
				printf("상위 메뉴로 돌아합니다.\n");
				break;
			}//end ch_config switch
			break;//case 0 종료

		case SPACEBAR://takeoff / landing
			if (takeoff == 0){
				sprintf(msg_buffer, "AT*REF=%d,%d\r", ++seq, 290718208); //Take off(이륙)
				//printf("\ntakeoff: %s\n", msg_buffer);
				takeoff = 1;
			}
			else{//takeoff == 1
				sprintf(msg_buffer, "AT*REF=%d,%d\r", ++seq, 290717696);
				//printf("\nlanding: %s\n", msg_buffer);
				takeoff = 0;
			}
			break;

		case '`'://emergency
			sprintf(msg_buffer, "AT*REF=%d,%d\rAT*REF=%d,%d\r", ++seq, 290717952, ++seq, 290717696);
			//memset(msg_buffer, 0, sizeof(msg_buffer));
			break;

			//move
			//AT*PCMD=(1)seq, (2)mode, (3)left/right, (4)front/back, (5)vertical speed, (6)angular speed, AT*PCMD_MAG(+(7)magneto psi, (8)magneto psi accuracy)
		case UP:
			vx = 0;
			vy = 0.1;
			vz = 0;
			vr = 0;

			sprintf(msg_buffer, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", ++seq, 1, *(int*)(&vx), *(int*)(&vy), *(int*)(&vz), *(int*)(&vr));
			printf("\nUP: %s\n", msg_buffer);
			break;

		case DOWN:
			vx = 0;
			vy = -0.1;
			vr = 0;
			vz = 0;
			sprintf(msg_buffer, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", ++seq, 1, *(int*)(&vx), *(int*)(&vy), *(int*)(&vz), *(int*)(&vr));
			printf("DOWN: %s\n", msg_buffer);
			break;
		case LEFT:
			vx = -0.1;
			vy = 0;
			vr = 0;
			vz = 0;
			sprintf(msg_buffer, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", ++seq, 1, *(int*)(&vx), *(int*)(&vy), *(int*)(&vz), *(int*)(&vr));
			printf("LEFT: %s\n", msg_buffer);
			break;

		case RIGHT:
			vx = 0.1;
			vy = 0;
			vr = 0;
			vz = 0;
			sprintf(msg_buffer, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", ++seq, 1, *(int*)(&vx), *(int*)(&vy), *(int*)(&vz), *(int*)(&vr));
			printf("RIGHT: %s\n", msg_buffer);
			break;

		case ESC:
			printf("\n종료합니다.\n");
			closesocket(ctr_sockfd);
			WSACleanup();
			exit(0);
			break;

		}//end switch


		// send command
		//     sendto(소켓,자료버퍼,자료의 길이, 플래그 - 상태값, 서버주소, 주소의 길이
		ctrRet = sendto(ctr_sockfd, msg_buffer, strlen(msg_buffer), 0, (SOCKADDR *)&ctr_server_addr, sizeof(SOCKADDR_IN));

		if (ctrRet == SOCKET_ERROR){
			printf("sendto error\n");
			closesocket(ctr_sockfd);
			return;
		}
		printf("MSG_BUFFER: %s\n", msg_buffer);
		memset(msg_buffer, 0, sizeof(msg_buffer));

		// hovering
		sprintf(msg_buffer, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", ++seq, 0, 0, 0, 0, 0);//hovering

		//     sendto(소켓,자료버퍼,자료의 길이, 플래그 - 상태값, 서버주소, 주소의 길이
		ctrRet = sendto(ctr_sockfd, msg_buffer, strlen(msg_buffer), 0, (SOCKADDR *)&ctr_server_addr, sizeof(SOCKADDR_IN));
		if (ctrRet == SOCKET_ERROR) {
			printf("sendto error\n");
			closesocket(ctr_sockfd);
			return;
		}
		memset(msg_buffer, 0, sizeof(msg_buffer));

	}//end for

	memset(msg_buffer, 0, sizeof(msg_buffer));

	// 응답을 기다립니다.

	int nFromLen;
	nFromLen = sizeof(struct sockaddr);

	// recvfrom(소켓, 받는버퍼, 버퍼의 길이, 플래그 - 상태값, 송신자의 주소를 받을 버퍼, 주소버퍼의 길이
	ctrRet = recvfrom(ctr_sockfd, msg_buffer, sizeof(msg_buffer), 0, (SOCKADDR *)&ctr_server_addr, &nFromLen);

	if (ctrRet == SOCKET_ERROR){
		printf("recv error\n");
		closesocket(ctr_sockfd);
		return;
	}

	// 받은 값을 표시 합니다
	printf("\nData received, 값을 받았습니다 : %s", msg_buffer);
	//end while
	closesocket(ctr_sockfd);
	return;

	closesocket(ctr_sockfd);
	WSACleanup();

	return 0;

}
