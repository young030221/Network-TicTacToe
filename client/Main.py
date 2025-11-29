import pygame
#사용할 class import
from UI import UI
from game_state import GameState 
from Network import Network

def main():
    pygame.init() # pygame 초기화

    game_state = GameState()
    ui = UI(game_state) #UI class 인자로 game_state의 주소를 연결해줌
        # game_state -> ui에 적용
    network = Network(game_state) #마찬가지로 Network에 game_state를 연결
        # network 의 정보 -> game_state

        # 전체적 흐름 network 정보 받기 -> game_state 상태 변경 -> ui 적용

    SERVER_IP = input("접속할 서버 IP를 입력하세요: ")
    network.connect(SERVER_IP,7777) #서버 접속

    running = True # pygame의 루프를 진행 하기 위한 flag 신호
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: # 이벤트가 QUIT이면<- ex)ui의 창 닫기 버튼
                running = False  # 게임 종료.

            #마우스 클릭
            if event.type == pygame.MOUSEBUTTONDOWN: # 마우스 클릭마다
                #게임이 진행중이고 자신의 턴인 경우에만 click 허용.
                if not game_state.game_end and game_state.turn == game_state.my_shape:
                    pos = ui.get_clicked_pos(event.pos)
                    if pos is not None and game_state.board[pos] == " ":
                        network.send(f"CLICK {pos}") #pos가 둘 수 있는 위치면 EX)"CLICK 3"  


            ui.render() # 저장된 정보를 화면으로 출력.
    pygame.quit() 

if __name__ == "__main__":
    main()

