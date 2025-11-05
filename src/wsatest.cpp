#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    int r = WSAStartup(MAKEWORD(2,2), &wsa);

    if (r != 0) {
        std::cout << "WSAStartup 실패: " << r << std::endl;
        return 1;
    }

    std::cout << "Winsock 초기화 성공!" << std::endl;

    WSACleanup();
    return 0;
}
