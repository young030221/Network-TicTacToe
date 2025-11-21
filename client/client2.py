# 기본적인 idea
# client는 입력과 출력을 둘다 동시에 가능해야한다.
# 즉, 멀티스레딩이 필요하다.
# 스레드 해야하는 부분 = 서버로부터 메시지를 받는것.
# why? : 입력을 기다리는 동안 출력이 blocking 되면 안되기 때문에.

import socket #TCP 소켓을 사용하기 위한 라이브러리
import threading # 멀티스레딩(동시에 여러일 Ex)출력,입력)을 위한 스레드 관련 라이브러리


SERVER_PORT= 7777 # 서버 포트 번호

#thread될 함수
def receive_thread(sock):
    while True:
        try:
            data = sock.recv(1024) #서버로 부터 메시지 받기.
            if not data:
                print("서버와 연결 끊어짐")
                break
            print("\n[Server]:",data.decode()) #sock은 데이터를 byte형태로 받기때문에
                #출력을 위해서 decode()로 문자열 형태로 변경
        except: #try문에서 에러 발생시
            break

def main():
    SERVER_IP = input("접속할 서버 IP를 입력하세요: ")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)#IPv4, TCP
    try:
        sock.connect((SERVER_IP, SERVER_PORT)) #서버에 연결 요청
            #CONNECT는 tuple 형태로 인자를 절달해야함.
    except:
        print("서버 연결 실패")
        return # 연결 시래시 종료
    print("서버 연결 성공!")
    
    #스레드 생성 : 함수명, 함수 인자(tuple 형태), 메인스레드 종료시 같이 종료
    thread = threading.Thread(target=receive_thread, args=(sock,),daemon=True)
    thread.start() #스레드 시작

    #메인 스레드 : 사용자 입력
    while True:
        msg = input("") #입력 대기
        if msg == "/quit": #사용자가 /quit 입력시 종료
            break
        sock.send(msg.encode()) #메시지 전송, 받을때와 반대로 문자열을 byte형태로 encoding

if __name__ == "__main__": #프로그램 시작점 설정.
    main()