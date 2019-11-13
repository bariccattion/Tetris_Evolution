//
// Created by Rafael on 13/11/2019.
//

#ifndef TETRIS_EVOLUTION_FUNCTIONS_H
#define TETRIS_EVOLUTION_FUNCTIONS_H

#include <iostream>
#include <cstdio>
#include <vector>

#include "functions.h"


int nScreenWidth = 120;            // Console Screen Size X (columns)
int nScreenHeight = 50;            // Console Screen Size Y (rows)

int nFieldWidth = 12;
int nFieldHeight = 18;
wstring tetromino[7];

int rotate(int px, int py, int r);

bool collisionCheck(int nTetromino, int nRotation, int nPosX, int nPosY, unsigned char *pField);


#endif //TETRIS_EVOLUTION_FUNCTIONS_H
