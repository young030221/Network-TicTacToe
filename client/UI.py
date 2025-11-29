import pygame

class UI:
    def __init__(self, game_state):
        pygame.init()
        self.game_state = game_state
        self.screen = pygame.display.set_mode((440,480)) # game window 크기 결정
        pygame.display.set_caption("Tic-tec-toe") # 제목

        self.shape_font = pygame.font.SysFont(None, 60) #모양 폰트 크기
        self.turn_font = pygame.font.SysFont(None, 30) #턴 폰트 크기

    def render(self):
        self.screen.fill((255, 255, 255))

        for i in range(3): 
            for j in range(3):
                x = j * 140 + 20 # 20, 160, 300
                y = i * 140 + 20 # 20, 160, 300
                #위의 각 해당 9칸의 위치에 120(20px우측내부 여백) 두고 두깨는 3px로 설정
                pygame.draw.rect(self.screen, (0,0,0), (x, y, 120, 120), 3) 

                shape = self.game_state.board[i*3 + j] #현재 위치의 모양을 추출.
                if shape != " ": # 빈칸이 아니면
                    # shape 모양 img에 넣음
                    img = self.shape_font.render(shape, True, (0,0,0)) # 문자를 그림으로 변환 True는 안티앨리어싱 기능
                    self.screen.blit(img,(x+35, y+25)) # 해당 칸의 중앙에 그림 shape그리기

        #
        turn_text = self.turn_font.render(f"Turn: {self.game_state.turn}", True, (0,0,0))
        self.screen.blit(turn_text, (20,440)) #아래쪽에 turn 그리기

        if self.game_state.game_end: #game이 끝나면
            #winner가 DRAW이면 DRAW! , 아니면 X WIN! or O WIN! 이 msg로 들어감
            msg = "DRAW!" if self.game_state.winner == "DRAW" else f"{self.game_state.winner} WIN!"
            result = self.shape_font.render(msg, True, (200,0,0)) #빨간색
            self.screen.blit(result,(150,440)) 

        #위에는 모두 보이지 안흔 화면에 그리고
        #결과는 flip할때 전체 출력
        pygame.display.flip()

    def get_clicked_pos(self,mouse_pos): # 
        x, y = mouse_pos
        if not (20 <= x <= 420 and 20 <= y <= 420):    
            return None #잘못된 범위는 none
        # 140 ~ 160, 280 ~ 300
        if 140 <= x <= 160 or 140 <= y <= 160 or 280 <= x <= 300 or 280 <= y <= 300:
            return None
        col = x // 140 # 각구간이 140px 로 나눠져있기때문에 해당 위치를 index로 변화하기 위한 것.
        row = y // 140
        return row *3 + col #python의 배열은 1차원이기때문에 2차원 좌표값을 1차원에 매칭시키는 로직.
                