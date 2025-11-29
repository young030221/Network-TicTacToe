# 실제 통신을 위한 code 
# network를 관리해서 다른 game_state,ui,main과 상호작용함

import socket
import threading

class Network:
    def __init__(self,game_state):
        self.sock = None # 서버 소켓 초기화
        self.game_state = game_state # GameState의 객체의 주소를 받아서 연결

    def connect(self, host, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #IPv4 , TCP
        self.sock.connect((host,port)) # host와 port를 받아서 서버와 연결을 시도

        thread = threading.Thread(
            target=self.receiveLoop, # receive는 thread로 따로 동시에 작동 될 수 있게 설정.
            daemon = True #main이 종료되면 같이 종료되게 
        )
        thread.start()

    def send(self,msg):
        if self.sock:
            self.sock.send(msg.encode()) #TCP는 바이트 스트림 형식이기때문에 문자열을 encode해서 서버로 전송.


    def receiveLoop(self):
        while True:
            try:
                data = self.sock.recv(1024) #1024바이트 만큼의 데이터 수신 <- blocking함수
                if not data: #data 가 비어있다 = 연결이 끊어짐
                    break #루프 종료
                msg = data.decode() #바이트 스트림 형식의 데이터를 문자열로 decode
                self.handle_message(msg) #받은 문자열을 handle_message 함수에 전달

            except: #받는 과정에서 오류가 발생
                self.sock.close() # 소켓 자체를 닫음
                break #루프 종료

    def handle_message(self,msg):
        #mouse로 올바른 위치(server측에서 판단)를 누르면 "ClICK 3 X" <-이런식으로 정보를 서버가 상대와 나에게 전달해줌.
        #tokenize
        parts = msg.split()
        #0번 = 명령 , 1번 = 0~8번 칸의 번호 , 2번 = 모양
        
        if parts[0] == "CLICK":
            pos = int(parts[1]) # 
            shape = parts[2]
            self.game_state.update_board(pos,shape) #game_state 객체에서 보드판 정보를 업데이트 해줌.

        #CLICK이후 바로 TURN 정보를 서버가 client에게 보내줌 턴 change
        #ex) "TURN X"
        elif parts[0] == "TURN":
            self.game_state.set_turn(parts[1])

        #ex) "WIN X"
        #누가 이겼는지 알려줌.
        elif parts[0] == "WIN":
            self.game_state.set_win(parts[1])

        #ex) "DRAW"
        #비김.
        elif parts[0] == "DRAW":
            self.game_state.set_draw()
        elif parts[0] == "CONNECTED":
            if parts[1] == "p1":
                self.game_state.my_shape = "X"
            else:
                self.game_state.my_shape = "O"
