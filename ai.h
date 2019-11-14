//
// Created by Rafael on 10/11/2019.
//
using namespace std;

#ifndef TETRIS_EVOLUTION_AI_H
#define TETRIS_EVOLUTION_AI_H
#include <float.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>

#include "functions.h"

class Scores {
public:
    int filledSpotCount;
    int weightedFilledSpotCount;
    int maximumAltitude;
    int holeCount;
    int deepestHole;
    int sumOfAllHoles;
};

class Genome {
public:
    int id;
    double fitness;
    double filledSpotCount;
    double weightedFilledSpotCount;
    double maximumAltitude;
    double holeCount;
    double deepestHole;
    double sumOfAllHoles;
};
class PossibleMoves {
public:
    int rotation;
    int xPos;
    double rating;
    Scores scores;
};

vector <PossibleMoves> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int nCurrentPiece);
void createInitialPopulation(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore);
void makeNextMove(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore);
void evaluateNextGenome(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore);
Genome makeChild(Genome mum, Genome dad);
void evolve();
int filledSpotCount(const unsigned char *pField);
int weightedFilledSpotCount(const unsigned char *pField);
int maximumAltitude(const unsigned char *pField);
int holeCount(const unsigned char *pField);
int deepestHole(const unsigned char *pField);
int sumOfAllHoles(const unsigned char *pField);
#endif //TETRIS_EVOLUTION_AI_H
