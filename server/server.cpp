    #include <iostream>
    #include <winsock2.h> // windows 소켓 프로그래밍을 하기 위한 함수 목록
    #include <ws2tcpip.h> // TCP/IP 관련 함수 목록
    #pragma comment(lib, "ws2_32.lib") // 실제 DLL과 연결하기 위한 라이브러리 링크

    #include "protocol.hpp"

    #define SERVER_PORT 7777 // 서버 포트 번호 일단 아무거나.

    using namespace std;

    int main(){
        WSADATA wsa;
        int r = WSAStartup(MAKEWORD(2,2), &wsa); //2.2 버전 winsock 사용
        if(r != 0){
            cout << "WSAStartup 실패" << endl;
            return 1;
        }
        // 여기까지 winsock 초기화 과정.
        SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0); // TCP 소켓 생성
        if (listenSocket == INVALID_SOCKET) {
            cout <<"socket 생성 실패" <<endl;
            WSACleanup(); // winsock 종료
            return 1;
        }

        //서버 주소 listenSocket 주소 설정
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET; // IPv4
        serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP로부터의 접속 허용
        serverAddr.sin_port = htons(SERVER_PORT); // 포트 번호 설정
            //ㄴ htons = 호스트 바이트 순서(리틀 엔디안)를 네트워크 바이트 순서(빅 엔디안)로 변환

        //server bind
        //구조체의 주소를 이용 listenSocket에 IP, port 할당, sizeof로 구조체 크기 전달
        if(bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
            cout << "bind 실패" << endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        if(listen(listenSocket, 2) == SOCKET_ERROR){ //client 2명이 game을 하기 때문에 2
            cout << "listen 실패" <<endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        } 

        cout << "서버 대기중. 접속을 기다리는 중..." << endl;

        // client 2명의 접속 받기.
        SOCKET Player1 = INVALID_SOCKET;
        SOCKET Player2 = INVALID_SOCKET;
        
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr); // sockaddr_in 구조체 크기
        //player1, player2가 접속할 때까지 무한 대기
        while(Player1 == INVALID_SOCKET){
            Player1 = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            if(Player1 == INVALID_SOCKET){
                cout << "Player1 접속 실패 / 대기중..." << endl;
            }
        }
        cout << "Player1 접속 성공!" << endl;
        while(Player2 == INVALID_SOCKET){
            Player2 = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            if(Player2 == INVALID_SOCKET){
                cout << "Player2 접속 실패 / 대기중..." << endl;
            }
        }
        cout << "Player2 접속 성공!" << endl;

        // 두 클라이언트에게 접속 성공 메시지 전송, msg, length, flags
        send(Player1, "CONNECTED p1", strlen("CONNECTED p1"), 0);
        send(Player2, "CONNECTED p2", strlen("CONNECTED p2"), 0);

        // client로 부터 받을 메시지를 저장할 버퍼
        char buffer[256];
        // 게임 루프 시작.
        while (true){
            //메시지 길이, byte 단위 char = 1byte, sizeof(buffer)-1 = \0을 위한 공간 확보
            int recvLen1 = recv(Player1, buffer, sizeof(buffer)-1,0); // Player1으로부터 메시지 수신
            if(recvLen1 > 0){
                buffer[recvLen1] = '\0'; // 수신된 메시지의 끝에 널 문자 추가.
                cout << "[Player1]: " << buffer << endl;
                send(Player2, buffer, recvLen1, 0); // Player2에게 Player 1의 메시지 전달
            }
            int recvLen2 = recv(Player2, buffer, sizeof(buffer)-1,0); // Player2으로부터 메시지 수신
            if(recvLen2 > 0){
                buffer[recvLen2] = '\0'; // 수신된 메시지의 끝에 널 문자 추가.
                cout << "[Player2]: " << buffer << endl;
                send(Player1, buffer, recvLen2, 0); // Player1에게 Player 2의 메시지 전달
            }
        }
        // 소켓 닫기 및 winsock 종료
        closesocket(Player1);
        closesocket(Player2);
        closesocket(listenSocket);
        WSACleanup(); // winsock 종료
        return 0;
    }

