//
// Created by Rafael on 10/10/2019.
//

#include "ai.h"
#include "wchar.h"
#include <string.h>
#include <cwchar>

const unsigned int seed = time(0);
mt19937_64 rng(seed);

double mutationRate = 0.05;
double mutationStep = 0.2;
int moveLimit = 500;

int filledSpotCount(const unsigned char *pField) {
    int filledSpots=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0)
                filledSpots++;
    }
    return filledSpots;
}

int maximumWellDepth(const unsigned char *pField) {
    int depthSize=0,maximumDepth=0,nAltitude=maximumAltitude(pField);
    for (int px = 1; px < nFieldWidth - 1; px++){
        int py=nFieldHeight-nAltitude-1;
        while(pField[py * nFieldWidth + px]==0){
            py++;
            depthSize=nFieldHeight-py-1;
        }
        if(maximumDepth<depthSize)
            maximumDepth=depthSize;
    }
    return maximumDepth;
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
    int nCount=0,pOldY;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0){
                nCount++;
                pOldY=py+1;
                while(pField[pOldY * nFieldWidth + px]==0){
                    nCount++;
                    pOldY++;
                }
            }
    }
    return nCount;
}

int connectedHolesCount(const unsigned char *pField) {
    int nCount=0,pOldY;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0){
                nCount++;
                pOldY=py+1;
            }
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

int rowTransitions(const unsigned char *pField) {
    int nCount=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && ((pField[(py+1) * nFieldWidth + px]!=0 && pField[(py+1) * nFieldWidth + px]!=9) || (pField[(py-1) * nFieldWidth + px]!=0 && pField[(py-1) * nFieldWidth + px]!=9)))
                nCount++;
    }
    return nCount;
}

int aggregateHeight(const unsigned char *pField) {
    int nHeightCount=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0){
                nHeightCount+=(nFieldHeight-py-1);
                break;
            }
    }
    return nHeightCount;
}

vector <PossibleMoves> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int nCurrentRotation,  int nCurrentPiece) {
    unsigned char *pFieldCopy = nullptr;
    pFieldCopy = new unsigned char[nFieldWidth * nFieldHeight];

    memcpy(pFieldCopy, pField, 12 * 18);
    vector<PossibleMoves> vectorPossibleMoves;
    int oldCurrentY=nCurrentY,oldCurrentX=nCurrentX,oldRotation=nCurrentRotation;
    bool failRotation=false,failXMove=false;

    // Each rotation
    for (int rotation = 0; rotation < 4; rotation++){
        nCurrentRotation=oldRotation;
        nCurrentY = oldCurrentY;
        nCurrentX = oldCurrentX;
        failRotation = false;
        // Test if piece can be rotated
        for (int i = 0; i <= rotation; i++)
            if (collisionCheck(nCurrentPiece, nCurrentRotation + i, nCurrentX, nCurrentY, pFieldCopy))
                nCurrentRotation += i;
            else {
                failRotation = true;
                break;
            }
        // Each position
        for (int position = -5; position < 5; position++) {
            if(!failRotation){
                // Restore stuff
                memcpy(pFieldCopy, pField, 12 * 18);
                nCurrentY = oldCurrentY;
                nCurrentX = oldCurrentX;
                failXMove=false;
                // Test if piece can be moved in X
                if(position>0){
                    for(int i=0;i<position; i++)
                        if(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY, pFieldCopy))
                            nCurrentX += 1;
                        else{
                            failXMove=true;
                            break;
                        }
                }else{
                    for(int i=0;i>position; i--)
                        if(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY, pFieldCopy))
                            nCurrentX -= 1;
                        else{
                            failXMove=true;
                            break;
                        }
                }
                if (!failXMove) {
                    // Move piece to the bottom of the field
                    while (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1, pFieldCopy))
                        nCurrentY++;

                    // Lock the piece in place
                    for (int px = 0; px < 4; px++)
                        for (int py = 0; py < 4; py++)
                            if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
                                pFieldCopy[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                    double rating = 0;

                    Scores thisScore;
                    thisScore.filledSpotCount = filledSpotCount(pFieldCopy);
                    thisScore.weightedFilledSpotCount = weightedFilledSpotCount(pFieldCopy);
                    thisScore.maximumAltitude = maximumAltitude(pFieldCopy);
                    thisScore.holeCount = holeCount(pFieldCopy);
                    thisScore.connectedHolesCount = connectedHolesCount(pFieldCopy);
                    thisScore.maximumWellDepth = maximumWellDepth(pFieldCopy);
                    //thisScore.deepestHole = deepestHole(pFieldCopy);
                    //thisScore.sumOfAllHoles = sumOfAllHoles(pFieldCopy);
                    thisScore.nLinesCleared = nLinesCleared;
                    thisScore.rowTransitions = rowTransitions(pFieldCopy);
    //                thisScore.yPiecePos = nCurrentY;
    //                thisScore.aggregateHeight = aggregateHeight(pFieldCopy);

                    rating += genomes.at(currentGenome).filledSpotCount*thisScore.filledSpotCount;
                    rating += genomes.at(currentGenome).weightedFilledSpotCount*thisScore.weightedFilledSpotCount;
                    rating += genomes.at(currentGenome).maximumAltitude * thisScore.maximumAltitude;
                    rating += genomes.at(currentGenome).holeCount * thisScore.holeCount;
                    rating += genomes.at(currentGenome).connectedHolesCount * thisScore.connectedHolesCount;
                    rating += genomes.at(currentGenome).maximumWellDepth * thisScore.maximumWellDepth;
                    //rating += genomes.at(currentGenome).deepestHole*thisScore.deepestHole;
                    //rating += genomes.at(currentGenome).sumOfAllHoles*thisScore.sumOfAllHoles;
                    rating += genomes.at(currentGenome).nLinesCleared * thisScore.nLinesCleared;
                    rating += genomes.at(currentGenome).rowTransitions * thisScore.rowTransitions;
    //                rating += genomes.at(currentGenome).yPiecePos * thisScore.yPiecePos;
    //                rating += genomes.at(currentGenome).aggregateHeight * thisScore.aggregateHeight;

                    if(gameOverCheck(pFieldCopy,nCurrentPiece))
                        rating-=500;

                    PossibleMoves thisMove;
                    thisMove.rating = rating;
                    thisMove.rotation = nCurrentRotation;
                    thisMove.xPos = nCurrentX;
                    thisMove.yPos = nCurrentY;
                    thisMove.scores = thisScore;

                    vectorPossibleMoves.push_back(thisMove);
                }
            }
        }
    }
    delete[] pFieldCopy;
    return vectorPossibleMoves;
}

// Creates the initial population of genomes, each with random genes.

void createInitialPopulation(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore) {
    uniform_real_distribution<double> range(-45000.0, 45000.0);
    Genome thisGenome;
    // For a given population size
    for (int i = 0; i < populationSize+1; i++) {
        // Randomly initialize the values that make up a genome
        // These are all weight values that are updated through evolution
        thisGenome.id=range(rng);
        thisGenome.fitness=-1;
        thisGenome.filledSpotCount=range(rng);
        thisGenome.weightedFilledSpotCount=range(rng);
        thisGenome.maximumAltitude=range(rng);
        thisGenome.holeCount=range(rng);
        thisGenome.connectedHolesCount=range(rng);
        thisGenome.maximumWellDepth=range(rng);
        //thisGenome.deepestHole=range(rng);
        //thisGenome.sumOfAllHoles=range(rng);
        thisGenome.nLinesCleared=range(rng);
        thisGenome.rowTransitions=range(rng);
//        thisGenome.yPiecePos=range(rng);
//        thisGenome.aggregateHeight=range(rng);
        genomes.push_back(thisGenome);
    }
    evaluateNext(pField, nCurrentX, nCurrentY, nCurrentRotation, nCurrentPiece, nScore);
}

void makeNextMove(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation,  int nCurrentPiece, int nScore) {
    movesTaken++;
    double highestRating=-DBL_MAX,highestMove=0;
    // Get all the possible moves
    vector<PossibleMoves> possibleMoves = getAllPossibleMoves(pField,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece);

    // Get best move rated by this genome
    for (int i = 0; i < possibleMoves.size(); i++) {
        if(possibleMoves.at(i).rating>highestRating) {
            highestRating = possibleMoves.at(i).rating;
            highestMove=i;
        }
    }

    // Do move
    PossibleMoves bestMove = possibleMoves.at(highestMove);
    nCurrentRotation=bestMove.rotation;
    nCurrentX=bestMove.xPos;
    nCurrentY=bestMove.yPos;
}

void evaluateNext(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore) {
    nTimesPlayed++;
    genomes.at(currentGenome).fitness+=movesTaken;

    if(nTimesPlayed == maxTimePlayed){
        genomes.at(currentGenome).fitness/=nTimesPlayed;
        nTimesPlayed=0;
        currentGenome++;
        if (currentGenome == genomes.size())
            evolve();
    }
    // Reset moves taken
    movesTaken = 0;
    // And make the next move
    makeNextMove(pField,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece,nScore);
}

void evolve() {
    currentGenome = 0;
    generation++;
    bGameOver=true;

    // Sort genomes by fitness
    for (int i = 0; i < genomes.size()-1; i++)
        for (int j = 0; j < genomes.size()-i-1; j++)
            if (genomes.at(j).fitness < genomes.at(j+1).fitness){
                Genome temp = genomes.at(j);
                genomes.at(j) = genomes.at(j+1);
                genomes.at(j+1) = temp;
            }

   // Add a copy of the fittest genome to the nobles list and sort it
   noble.push_back(genomes.at(0));
   if(noble.size()>1){
    for (int i = 0; i < noble.size()-1; i++)
        for (int j = 0; j < noble.size()-i-1; j++)
            if(noble.size()>1)
                if (noble.at(j).fitness < noble.at(j+1).fitness){
                    Genome temp = noble.at(j);
                    noble.at(j) = noble.at(j+1);
                    noble.at(j+1) = temp;
                }
   }
   if(noble.size()>5)
       noble.pop_back();


    // Kill the worst of the population
    while(genomes.size() > populationSize/2)
        genomes.pop_back();

    // Create children array
    vector<Genome> children;
    uniform_int_distribution<int> rangeChild(0, genomes.size()-1);

    for(int i=0;i<populationSize/4;i++) {
        children.push_back(makeChild(genomes.at(rangeChild(rng)), genomes.at(rangeChild(rng))));
        children.push_back(makeChild(genomes.at(rangeChild(rng)), genomes.at(0)));
    }

    // Add nobles to genomes array
    for(Genome n : noble){
        n.fitness=-1;
        genomes.pop_back();
        genomes.push_back(n);
    }

    // Add children to genomes array
    for(Genome child : children)
        genomes.push_back(child);

}

Genome makeChild(Genome &mum, Genome &dad) {
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    uniform_real_distribution<double> rangeCrossingOver(0, 1);
    uniform_real_distribution<double> rangeAjustment(0, mutationStep);

    Genome child;
    double choice;
    int choiceRound;

    // Weighted
    child.maximumAltitude=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.maximumAltitude + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.maximumAltitude;
    child.holeCount=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.holeCount + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.holeCount;
    child.connectedHolesCount=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.connectedHolesCount + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.connectedHolesCount;
    child.maximumWellDepth=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.maximumWellDepth + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.maximumWellDepth;
    child.nLinesCleared=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.nLinesCleared + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.nLinesCleared;
    child.rowTransitions=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.rowTransitions + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.rowTransitions;
//    child.yPiecePos=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.yPiecePos + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.yPiecePos;
//    child.aggregateHeight=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.aggregateHeight + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.aggregateHeight;
    //child.deepestHole=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.deepestHole + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.deepestHole;
    child.weightedFilledSpotCount=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.weightedFilledSpotCount + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.weightedFilledSpotCount;
    child.filledSpotCount=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.filledSpotCount + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.filledSpotCount;
    //child.sumOfAllHoles=((mum.fitness)/((mum.fitness)+(dad.fitness)))*mum.sumOfAllHoles + ((dad.fitness)/((mum.fitness)+(dad.fitness)))*dad.sumOfAllHoles;

    child.fitness=-1;

    // We mutate each parameter using our mutationstep
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.maximumAltitude+=rangeAjustment(rng) : child.maximumAltitude-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.connectedHolesCount+=rangeAjustment(rng) : child.connectedHolesCount-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.holeCount+=rangeAjustment(rng) : child.holeCount-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.maximumWellDepth+=rangeAjustment(rng) : child.maximumWellDepth-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.nLinesCleared+=rangeAjustment(rng) : child.nLinesCleared-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.weightedFilledSpotCount+=rangeAjustment(rng) : child.weightedFilledSpotCount-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.filledSpotCount+=rangeAjustment(rng) : child.filledSpotCount-=rangeAjustment(rng);
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        choice=rangeCrossingOver(rng);choiceRound=round(choice);
        choiceRound ? child.rowTransitions+=rangeAjustment(rng) : child.rowTransitions-=rangeAjustment(rng);
    }

    return child;
}
