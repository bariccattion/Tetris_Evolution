//
// Created by Rafael on 10/11/2019.
//
using namespace std;

#ifndef TETRIS_EVOLUTION_AI_H
#define TETRIS_EVOLUTION_AI_H

#include <iostream>
#include <cstdio>
#include <vector>

#include "functions.h"

wstring tetromino[7];

int filledSpotCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
int weightedFilledSpotCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
int maximumAltitude(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
int holeCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
int deepestHole(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
int sumOfAllHoles(const unsigned char *pField, int nFieldWidth, int nFieldHeight);
#endif //TETRIS_EVOLUTION_AI_H
