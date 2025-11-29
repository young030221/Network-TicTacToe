    #include <iostream>
    #include "game.hpp"

    Game::Game(){ // 생성자
        reset();
    }

    void Game::reset(){ //board 판과 turn을 초기화
        for(int i = 0; i<9;i++)
            board[i] = ' ';
        turn = 'X';
    }

    char Game::getTurn() { //turn을 반환
        return turn;
    }

    bool Game::isValid(int pos){ //해당 위치가 가능한지 판단
        //범위 안에서 해당 칸이 비어있을경우.
        // 단 client 측에서 이미 해당 경우 send하지 않게 설계되어있긴 하지만 server측에서도 구현.
        return (pos>=0 && pos < 9 && board[pos] == ' ');
    }
    //server에서 isValid인 경우 board를 수정.
    void Game::applyClick(int pos){
        board[pos] = turn;
    }

    bool Game::checkWin(char p){
        const int win[8][3] = {
            //가로
            {0,1,2},{3,4,5},{6,7,8},
            //세로
            {0,3,6},{1,4,7},{2,5,8},
            //대각
            {0,4,8},{2,4,6}
        };

        for (auto w : win){
            //승리조건 중 하나라도 해당한다면
            if(board[w[0]] == p && board[w[1]] == p && board[w[2]] == p)
                return true;
        }
        return false;
    }

    bool Game::checkDraw() {
        for(int i = 0 ;i<9; i++){
            if(board[i] == ' ') return false;
        }
        return true;
    }

    void Game::toggleTurn(){
        turn = (turn == 'X') ? 'O' : 'X'; 
    }

    string Game::getBoardString(){
        return string(board,9); // network에 보드 상태를 보내기 간단하게 하기 위해 string을 변환.
        //ex XO XOO  X <- 공백 포함 9칸
    }