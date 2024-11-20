#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <thread>
#include <random>
using namespace std;

// Kích thước màn chơi
const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 18;

// Enum cho màu sắc
enum Colors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7
};

// Các khối tetromino
vector<vector<vector<bool>>> tetrominos = {
    // I
    {{1,1,1,1}},
    // O
    {{1,1},
     {1,1}},
    // T
    {{0,1,0},
     {1,1,1}},
    // S
    {{0,1,1},
     {1,1,0}},
    // Z
    {{1,1,0},
     {0,1,1}},
    // J
    {{1,0,0},
     {1,1,1}},
    // L
    {{0,0,1},
     {1,1,1}}
};

// Màu sắc cho các tetromino
vector<int> tetrominoColors = {
    CYAN,   // I
    YELLOW, // O
    MAGENTA,// T
    GREEN,  // S
    RED,    // Z
    BLUE,   // J
    WHITE   // L
};

class Game {
    vector<vector<bool>> board;
    vector<vector<int>> colorBoard;
    vector<vector<bool>> currentPiece;
    int currentX, currentY;
    int currentColor;
    int score = 0;
    int level = 1;
    int linesCleared = 0;
    bool gameOver = false;
    
public:
    Game() : board(BOARD_HEIGHT, vector<bool>(BOARD_WIDTH, false)),
             colorBoard(BOARD_HEIGHT, vector<int>(BOARD_WIDTH, BLACK)) {
        spawnNewPiece();
    }

    void spawnNewPiece() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, tetrominos.size() - 1);
        int pieceIndex = dis(gen);
        currentPiece = tetrominos[pieceIndex];
        currentColor = tetrominoColors[pieceIndex];
        currentX = BOARD_WIDTH / 2 - currentPiece[0].size() / 2;
        currentY = 0;
        
        if (!canMove(currentX, currentY)) {
            gameOver = true;
        }
    }

    bool canMove(int newX, int newY) {
        for (int y = 0; y < currentPiece.size(); y++) {
            for (int x = 0; x < currentPiece[y].size(); x++) {
                if (currentPiece[y][x]) {
                    int boardX = newX + x;
                    int boardY = newY + y;
                    
                    if (boardX < 0 || boardX >= BOARD_WIDTH || 
                        boardY >= BOARD_HEIGHT ||
                        (boardY >= 0 && board[boardY][boardX])) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void mergePiece() {
        for (int y = 0; y < currentPiece.size(); y++) {
            for (int x = 0; x < currentPiece[y].size(); x++) {
                if (currentPiece[y][x]) {
                    board[currentY + y][currentX + x] = true;
                    colorBoard[currentY + y][currentX + x] = currentColor;
                }
            }
        }
        clearLines();
        spawnNewPiece();
    }

    void clearLines() {
        int clearedLines = 0;
        for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
            bool lineFull = true;
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (!board[y][x]) {
                    lineFull = false;
                    break;
                }
            }
            if (lineFull) {
                clearedLines++;
                score += 100 * level;
                for (int y2 = y; y2 > 0; y2--) {
                    board[y2] = board[y2-1];
                    colorBoard[y2] = colorBoard[y2-1];
                }
                board[0] = vector<bool>(BOARD_WIDTH, false);
                colorBoard[0] = vector<int>(BOARD_WIDTH, BLACK);
                y++; // Kiểm tra lại dòng hiện tại
            }
        }
        
        linesCleared += clearedLines;
        level = 1 + linesCleared / 10;
    }

    void moveLeft() {
        if (canMove(currentX - 1, currentY))
            currentX--;
    }

    void moveRight() {
        if (canMove(currentX + 1, currentY))
            currentX++;
    }

    void rotate() {
        vector<vector<bool>> rotated(currentPiece[0].size(), 
            vector<bool>(currentPiece.size()));
        
        for (int y = 0; y < currentPiece.size(); y++) {
            for (int x = 0; x < currentPiece[y].size(); x++) {
                rotated[x][currentPiece.size()-1-y] = currentPiece[y][x];
            }
        }
        
        auto oldPiece = currentPiece;
        currentPiece = rotated;
        if (!canMove(currentX, currentY)) {
            currentPiece = oldPiece;
        }
    }

    bool moveDown() {
        if (canMove(currentX, currentY + 1)) {
            currentY++;
            return true;
        }
        mergePiece();
        return false;
    }

    void hardDrop() {
        while(moveDown()) {}
    }

    void draw() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        system("cls");
        cout << "╔════════════════════════╗\n";
        cout << "║ Score: " << score << "\n";
        cout << "║ Level: " << level << "\n";
        cout << "║ Lines: " << linesCleared << "\n";
        cout << "╚════════════════════════╝\n\n";
        
        vector<vector<bool>> display = board;
        vector<vector<int>> colorDisplay = colorBoard;
        
        for (int y = 0; y < currentPiece.size(); y++) {
            for (int x = 0; x < currentPiece[y].size(); x++) {
                if (currentPiece[y][x] && currentY + y >= 0) {
                    display[currentY + y][currentX + x] = true;
                    colorDisplay[currentY + y][currentX + x] = currentColor;
                }
            }
        }
        
        cout << "╔";
        for (int x = 0; x < BOARD_WIDTH * 2; x++) cout << "═";
        cout << "╗\n";
        
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            cout << "║";
            for (int x = 0; x < BOARD_WIDTH; x++) {
                SetConsoleTextAttribute(hConsole, colorDisplay[y][x]);
                cout << (display[y][x] ? "██" : "  ");
            }
            SetConsoleTextAttribute(hConsole, WHITE);
            cout << "║\n";
        }
        
        cout << "╚";
        for (int x = 0; x < BOARD_WIDTH * 2; x++) cout << "═";
        cout << "╝\n";
        
        cout << "\nControls:\n";
        cout << "A - Move Left\n";
        cout << "D - Move Right\n";
        cout << "S - Move Down\n";
        cout << "W - Rotate\n";
        cout << "Space - Hard Drop\n";
    }

    bool isGameOver() { return gameOver; }
    int getLevel() { return level; }
};

int main() {
    // Thiết lập console
    system("title Tetris");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    Game game;
    auto lastFall = chrono::steady_clock::now();

    while (!game.isGameOver()) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'a': case 'A': game.moveLeft(); break;
                case 'd': case 'D': game.moveRight(); break;
                case 's': case 'S': game.moveDown(); break;
                case 'w': case 'W': game.rotate(); break;
                case ' ': game.hardDrop(); break;
            }
        }

        auto now = chrono::steady_clock::now();
        int fallDelay = max(50, 500 - (game.getLevel() - 1) * 50);
        if (chrono::duration_cast<chrono::milliseconds>(now - lastFall).count() > fallDelay) {
            game.moveDown();
            lastFall = now;
        }

        game.draw();
        Sleep(30);
    }

    SetConsoleTextAttribute(hConsole, WHITE);
    cout << "\n╔════════════════════════╗\n";
    cout << "║      GAME OVER!        ║\n";
    cout << "╚════════════════════════╝\n";
    return 0;
} 