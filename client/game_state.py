class GameState:
    def __init__(self):
        self.board = [" "]*9 #보드를 9칸 초기화
        self.turn = "X" #X먼저 시작
        self.game_end = False
        self.winner = None

    def update_board(self, pos, shape): #위치를 해당 모양으로 변경
        self.board[pos] = shape

    def set_turn(self, shape): #turn을 change
        self.turn = shape

    def set_win(self, shape): #shape이 이김
        self.game_end = True
        self.winner = shape #winner 가 win이면 이긴 사람 모양을 비기면 DRAW를

    def set_draw(self): #비김
        self.game_end = True
        self.winner = "DRAW"