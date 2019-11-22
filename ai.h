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
#include <algorithm>

#include "functions.h"

class Scores {
public:
    int filledSpotCount;
    int weightedFilledSpotCount;
    int maximumAltitude;
    int holeCount;
    int connectedHolesCount;
    int maximumWellDepth;
    //int deepestHole;
    //int sumOfAllHoles;
    int nLinesCleared;
    int rowTransitions;
    //int aggregateHeight;
    //int yPiecePos;
};

class Genome {
public:
    int id;
    double fitness;
    double filledSpotCount;
    double weightedFilledSpotCount;
    double maximumAltitude;
    double holeCount;
    double connectedHolesCount;
    double maximumWellDepth;
    //double deepestHole;
    //double sumOfAllHoles;
    double nLinesCleared;
    double rowTransitions;
    //double aggregateHeight;
    //double yPiecePos;
};
class PossibleMoves {
public:
    int rotation;
    int xPos;
    int yPos;
    double rating;
    Scores scores;
};

inline int movesTaken = 0;
inline int currentGenome = 0;
inline int generation=0;
inline int populationSize = 250;
inline int nTimesPlayed=0;
inline int maxTimePlayed=5;
inline vector<Genome> genomes;
inline vector<Genome> noble;

vector <PossibleMoves> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int nCurrentRotation, int nCurrentPiece);
void createInitialPopulation(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore);
void makeNextMove(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore);
void evaluateNext(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore);
Genome makeChild(Genome &mum, Genome &dad);
void evolve();

int maximumWellDepth(const unsigned char *pField);
int connectedHolesCount(const unsigned char *pField);
int aggregateHeight(const unsigned char *pField);
int rowTransitions(const unsigned char *pField);
int filledSpotCount(const unsigned char *pField);
int weightedFilledSpotCount(const unsigned char *pField);
int maximumAltitude(const unsigned char *pField);
int holeCount(const unsigned char *pField);
int deepestHole(const unsigned char *pField);
int sumOfAllHoles(const unsigned char *pField);
#endif //TETRIS_EVOLUTION_AI_H
