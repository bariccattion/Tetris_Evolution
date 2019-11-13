//
// Created by Rafael on 13/11/2019.
//

#include "functions.h"

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
                }
            }
        }

    return true;
}
