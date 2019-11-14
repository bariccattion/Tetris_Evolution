//
// Created by Rafael on 13/11/2019.
//

#include "functions.h"
#include <random>
#include <ctime>

int bagRandomizer(vector<int> &pool) {
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    uniform_int_distribution<int> range(0, pool.size() - 1);
    // Choose a random tetromino from pool
    int pos=range(rng);
    int piece = pool.at(pos);
    pool.erase(pool.begin() + pos);

    if (pool.empty())
        pool = {0, 1, 2, 3, 4, 5, 6};

    return piece;
}

int rotate(int px, int py, int r) {
    //Rotation matrix in 1d vector format
    int pi = 0;
    switch (r % 4) {
        case 0: //0
            pi = py * 4 + px;
            break;

        case 1: //90
            pi = 12 + py - (px * 4);
            break;

        case 2: //180
            pi = 15 - (py * 4) - px;
            break;

        case 3: //270
            pi = 3 - py + (px * 4);
            break;
    }
    return pi;
}

bool collisionCheck(int nTetromino, int nRotation, int nPosX, int nPosY, const unsigned char *pField) {

    wstring tetromino[7];
    tetromino[0].append(L"..X...X...X...X."); // I
    tetromino[1].append(L"..X..XX...X....."); // T
    tetromino[2].append(L".....XX..XX....."); // O
    tetromino[3].append(L"..X..XX..X......"); // S
    tetromino[4].append(L".X...XX...X....."); // Z
    tetromino[5].append(L".X...X...XX....."); // J
    tetromino[6].append(L"..X...X..XX....."); // L

    // All Field cells >0 are occupied
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++) {
            // Get index into piece
            int pi = rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            // Check that test is in bounds.
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                    // In Bounds so do collision check
                    if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
                        return false; // fail on first hit
                }
            }
        }

    return true;
}

bool gameOverCheck(const unsigned char *pFieldRef, int currentFigure) {
    int nCurrentX = (nFieldWidth / 2) - 2;
    int nCurrentY = 0;
    int nCurrentRotation = 0;

    // If piece does not fit straight away, game over!
    bool bGameOver = !collisionCheck(currentFigure, nCurrentRotation, nCurrentX, nCurrentY, pFieldRef);
    return bGameOver;
}


void resetGame() {

    int nCurrentRotation = 0;
    int nCurrentX = (nFieldWidth / 2) - 2;
    int nCurrentY = 0;
    int nSpeed = 20;
    int nSpeedCount = 0;
    bool bForceDown = false;
    bool bRotateHold = true;
    int nLinesCleared=0;
    std::vector<int> vLines;
    bool bGameOver = false;

    /*pFieldLocal = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer
    for (int x = 0; x < nFieldWidth; x++) // Board Boundary
        for (int y = 0; y < nFieldHeight; y++)
            pFieldLocal[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;*/
}
