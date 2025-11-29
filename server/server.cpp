    
    #include <iostream>
    #include <winsock2.h> // windows 소켓 프로그래밍을 하기 위한 함수 목록
    #include <windows.h> 
    #include <string>
    #include <thread> // 멀티스레딩을 위한 스레드 관련 함수 목록
    #include <ws2tcpip.h> // TCP/IP 관련 함수 목록
    #pragma comment(lib, "ws2_32.lib") // 실제 DLL과 연결하기 위한 라이브러리 링크

    #include "game.hpp"


    #define SERVER_PORT 7777 // 서버 포트 번호 일단 아무거나.

    using namespace std;

    Game game;
    
    void client_thread(     SOCKET* mySock,
                            SOCKET* otherSock,
                            const char* myName, 
                            const char* otherName, 
                            SOCKET listenSocket, 
                            sockaddr_in& clientAddr, 
                            int& clientAddrSize,
                            Game *game 
    ){
        // 게임 루프 시작.
        // client로 부터 받을 메시지를 저장할 버퍼
        char buffer[256];
        while (true){
            //메시지 길이, byte 단위 char = 1byte, sizeof(buffer)-1 = \0을 위한 공간 확보
            int recvLen = recv(*mySock, buffer, sizeof(buffer)-1,0); // Player로부터 메시지 수신
            //player가 접속 종료시 recvLen <=0 재접속 대기
            if(recvLen <= 0){
                cout << myName <<" 접속 종료됨" << endl;
                if(*otherSock != INVALID_SOCKET){ // 다른 플레이어가 접속 중이라면 알려주기
                    string msg = string(myName) + " 연결 끊김"; //char* to string 
                    send(*otherSock, msg.c_str(), msg.length(), 0);
                }

                closesocket(*mySock);
                *mySock = INVALID_SOCKET;

                //재접속을 대기
                while(*mySock == INVALID_SOCKET){
                    *mySock = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
                    if(*mySock == INVALID_SOCKET){
                        cout << myName << " 접속 실패 / 대기중..." << endl;
                    }
                }

                //재접속 성공 알려주기
                cout << myName << " 재접속 성공!" << endl;
                string recon = string(myName) + " 재접속 성공!";
                send(*mySock, recon.c_str(), recon.length(), 0); //자기 자신에게 재접속 성공 메시지 전송
                if(*otherSock != INVALID_SOCKET){ // 다른 플레이어가 접속 중이라면 알려주기
                    string msg = string(myName) + " 재접속됨"; //char* to string 
                    send(*otherSock, msg.c_str(), msg.length(), 0);
                }
            }
            //정상 메시지 수신
            buffer[recvLen] = '\0'; // 수신된 메시지의 끝에 널 문자 추가.
            cout << "[" << myName << "]: " << buffer << endl;
            ///////////////////////////////////////
            //---------game logic처리 ------------//
            ///////////////////////////////////////
            if(strncmp(buffer,"CLICK",5) == 0){ //buffer의 내용이  CLICK인 경우
                int pos = atoi(buffer + 6); //click 위치 반환.
                   
                // 현재 플레이어에 따라서 X or O
                char myShape = (strcmp(myName, "Player1")==0 ? 'X' : 'O');

                // 현재 턴이 아니거나 click 위치가 가능하지 않으면
                if(game->getTurn() != myShape ||!game->isValid(pos))
                    continue;
                
                game->applyClick(pos);
                
                //적용된 변화에 대한 msg를 만들어서 각 client에게 전달
                string msg = string("CLICK ") + to_string(pos) + " " + myShape;
                send(*mySock,msg.c_str(), msg.size(),0);
                send(*otherSock,msg.c_str(),msg.size(),0);

                //이겼는지
                if(game->checkWin(myShape)){
                    string winmsg = string("WIN ") + myShape;
                    send(*mySock, winmsg.c_str() , winmsg.size(),0);
                    string losemsg = "LOSE";
                    send(*otherSock, losemsg.c_str(),losemsg.size(),0);
                    continue;
                }
                //비겼는지
                if(game->checkDraw()){
                    string drawmsg = "DRAW";
                    send(*mySock, drawmsg.c_str(),drawmsg.size(),0);
                    send(*otherSock, drawmsg.c_str(),drawmsg.size(),0);
                    continue;
                }

                //turn change
                game->toggleTurn();
                char next = game->getTurn();

                string tmsg = string("TURN ") + next; // 누구 턴인지 알려주기.
                send(*mySock, tmsg.c_str(), tmsg.size(), 0);
                send(*otherSock, tmsg.c_str(), tmsg.size(), 0);
            }
        }
    }

    int main(){
        SetConsoleOutputCP(CP_UTF8); // 콘솔 출력을 UTF-8로 설정
        SetConsoleCP(CP_UTF8); // 콘솔 입력을 UTF-8로 설정
        
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

        // 클라이언트 스레드 시작
        thread t1(client_thread,
                  &Player1, 
                  &Player2, 
                  "Player1", 
                  "Player2", 
                  listenSocket, 
                  ref(clientAddr), 
                  ref(clientAddrSize),
                  &game
        );

        thread t2(client_thread,
                  &Player2, 
                  &Player1, 
                  "Player2", 
                  "Player1", 
                  listenSocket, 
                  ref(clientAddr), 
                  ref(clientAddrSize),
                  &game
        );

        t1.join(); // 이 스레드들이 실행되면 main은 여기서 접속이 끊어질때 까지 대기
        t2.join();


        // 소켓 닫기 및 winsock 종료
        closesocket(Player1);
        closesocket(Player2);
        closesocket(listenSocket);
        WSACleanup(); // winsock 종료
        return 0;
    }

