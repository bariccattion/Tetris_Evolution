//
// Created by Rafael on 10/11/2019.
//
using namespace std;

#ifndef TETRIS_EVOLUTION_AI_H
#define TETRIS_EVOLUTION_AI_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include <ctime>

#include "functions.h"

const unsigned int seed = time(0);
mt19937_64 rng(seed);

int populationSize = 10;
int currentGenome = 0;
int generation=0;

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
    int filledSpotCount;
    int weightedFilledSpotCount;
    int maximumAltitude;
    int holeCount;
    int deepestHole;
    int sumOfAllHoles;
};
vector<Genome> genomes;

class PossibleMoves {
public:
    int rotation;
    int xPos;
    double rating;
    Scores scores;
};



int filledSpotCount(const unsigned char *pField);
int weightedFilledSpotCount(const unsigned char *pField);
int maximumAltitude(const unsigned char *pField);
int holeCount(const unsigned char *pField);
int deepestHole(const unsigned char *pField);
int sumOfAllHoles(const unsigned char *pField);
#endif //TETRIS_EVOLUTION_AI_H
