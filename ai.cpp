//
// Created by Rafael on 10/10/2019.
//

#include "ai.h"

int filledSpotCount(const unsigned char *pField) {
    int filledSpots=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0)
                filledSpots++;
    }
    return filledSpots;
}

int weightedFilledSpotCount(const unsigned char *pField) {
    int filledSpots=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0)
                filledSpots=filledSpots+1*(nFieldHeight-py-1);
    }
    return filledSpots;
}

int maximumAltitude(const unsigned char *pField) {
    int maxBlockPos=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0 && (nFieldHeight-py-1)>maxBlockPos)
                maxBlockPos=(nFieldHeight-py-1);
    }
    return maxBlockPos;
}

int holeCount(const unsigned char *pField) {
    int nCount=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0)
                nCount++;
    }
    return nCount;
}

int deepestHole(const unsigned char *pField) {
    int deepestHolePos=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0 && pField[(py) * nFieldWidth + px+1]!=0 && pField[(py) * nFieldWidth + px-1]!=0)
                deepestHolePos=(nFieldHeight-py-1);
    }
    return deepestHolePos;
}

int sumOfAllHoles(const unsigned char *pField) {
    int sumOfAllHoles=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0 && pField[(py) * nFieldWidth + px+1]!=0 && pField[(py) * nFieldWidth + px-1]!=0)
                sumOfAllHoles+=(nFieldHeight-py-1);
    }
    return sumOfAllHoles;
}

vector<PossibleMoves> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int nCurrentPiece) {
    unsigned char *pFieldCopy = (unsigned char *) malloc(12 * 18);
    int nCurrentRotation = 0, oldCurrentY;
    vector<PossibleMoves> vectorPossibleMoves;
    memcpy(pFieldCopy, pField, 12 * 18);


    // Each position
    for (int position = -5; position < 5; position++) {
        // Test if piece can be moved in X
        if(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + position, nCurrentY, pFieldCopy)){
            nCurrentX += position;

            // Each rotation
            for (int rotation = 0; rotation < 4; rotation++) {
                //Do stuff before rotating
                // Move piece to the bottom of the field
                oldCurrentY=nCurrentY;
                while(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1, pField))
                    nCurrentY++;
                // Lock the piece in place
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
                            pFieldCopy[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                double rating=0;

                Scores thisScore;
                thisScore.filledSpotCount=filledSpotCount(pFieldCopy);
                thisScore.weightedFilledSpotCount=weightedFilledSpotCount(pFieldCopy);
                thisScore.maximumAltitude=maximumAltitude(pFieldCopy);
                thisScore.holeCount=holeCount(pFieldCopy);
                thisScore.deepestHole=deepestHole(pFieldCopy);
                thisScore.sumOfAllHoles=sumOfAllHoles(pFieldCopy);

                rating+=genomes.at(currentGenome).filledSpotCount*thisScore.filledSpotCount;
                rating+=genomes.at(currentGenome).weightedFilledSpotCount*thisScore.weightedFilledSpotCount;
                rating+=genomes.at(currentGenome).maximumAltitude*thisScore.maximumAltitude;
                rating+=genomes.at(currentGenome).holeCount*thisScore.holeCount;
                rating+=genomes.at(currentGenome).deepestHole*thisScore.deepestHole;
                rating+=genomes.at(currentGenome).sumOfAllHoles*thisScore.sumOfAllHoles;

                // If the move loses the game, lower its rating
                if (gameOverCheck(pFieldCopy,nCurrentPiece)) {
                    rating -= 500;
                }

                PossibleMoves thisMove;
                thisMove.rating=rating;
                thisMove.rotation=rotation;
                thisMove.xPos=position;
                thisMove.scores=thisScore;
                vectorPossibleMoves.push_back(thisMove);

                // Restore stuff
                memcpy(pFieldCopy, pField, 12 * 18);
                nCurrentY=oldCurrentY;

                // Test if piece can be rotated
                nCurrentRotation = (collisionCheck(nCurrentPiece, nCurrentRotation + rotation, nCurrentX, nCurrentY, pFieldCopy)) ? rotation : 0;
            }
        }
    }
    return vectorPossibleMoves;
}

// Creates the initial population of genomes, each with random genes.
void createInitialPopulation() {
    uniform_real_distribution<double> range(-1, 1);
    Genome thisGenome;
    // For a given population size
    for (int i = 0; i < populationSize; i++) {
        // Randomly initialize the values that make up a genome
        // These are all weight values that are updated through evolution
        thisGenome.id=range(rng);
        thisGenome.filledSpotCount=range(rng);
        thisGenome.weightedFilledSpotCount=range(rng);
        thisGenome.maximumAltitude=range(rng);
        thisGenome.holeCount=range(rng);
        thisGenome.deepestHole=range(rng);
        thisGenome.sumOfAllHoles=range(rng);
        genomes.push_back(thisGenome);
    }
    //evaluateNextGenome();
}

// Evaluates the next genome in the population. If there is none, evolves the population.
void evaluateNextGenome(vector<Genome> &genomes) {
    //increment index in genome array
    currentGenome++;
    //If there is none, evolves the population.
    if (currentGenome == genomes.size()) {
        //evolve();
    }
    //load current gamestate
    //loadState(roundState);
    //reset moves taken
    //movesTaken = 0;
    //and make the next move
    //makeNextMove();
}

