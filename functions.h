//
// Created by Rafael on 13/11/2019.
//
using namespace std;

#ifndef TETRIS_EVOLUTION_FUNCTIONS_H
#define TETRIS_EVOLUTION_FUNCTIONS_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include <ctime>

#include "functions.h"

inline wstring tetromino[7];
inline int SCREENHEIGHT = 50;
inline int SCREENWIDTH = 60;
inline int const nFieldWidth=12;
inline int const nFieldHeight=18;
inline int nLinesCleared=0;
inline bool bGameOver = false;
inline wchar_t *screen = new wchar_t[SCREENWIDTH*SCREENHEIGHT];

int bagRandomizer(vector<int> &pool);
int rotate(int px, int py, int r);
bool collisionCheck(int nTetromino, int nRotation, int nPosX, int nPosY, const unsigned char *pField);
bool gameOverCheck(const unsigned char *pField, int currentFigure);


#endif //TETRIS_EVOLUTION_FUNCTIONS_H
