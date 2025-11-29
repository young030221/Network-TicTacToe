#pragma once
#include <iostream>
#include <string>
using namespace std;

class Game{
private:
    char board[9];
    char turn; // X or O
public:
    Game();
    void  reset();
    char getTurn();
    bool isValid(int pos);
    void applyClick(int pos);
    bool checkWin(char player);
    bool checkDraw();
    void toggleTurn();
    string getBoardString();
};