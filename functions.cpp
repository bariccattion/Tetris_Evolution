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
                }else{
                    // Out of bounds?
                    return false;
                }
            }else{
                // Out of bounds?
                return false;
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

