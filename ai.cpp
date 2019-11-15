//
// Created by Rafael on 10/10/2019.
//

#include "ai.h"

const unsigned int seed = time(0);
mt19937_64 rng(seed);

double mutationRate = 0.05;
double mutationStep = 0.2;
int moveLimit = 500;
int populationSize = 10;

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

int rowTransitions(const unsigned char *pField) {
    int nCount=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && (pField[(py+1) * nFieldWidth + px]!=0 || pField[(py-1) * nFieldWidth + px]!=0))
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
    unsigned char *pFieldCopy = (unsigned char *) malloc(12 * 18);
    memcpy(pFieldCopy, pField, 12 * 18);
    vector<PossibleMoves> vectorPossibleMoves;
    int oldCurrentY;
    bool fail=false;

    // Each position
    for (int position = -5; position < 5; position++) {
        fail=false;
        // Test if piece can be moved in X
        if(position>0){
            for(int i=0;i<position; i++)
                if(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + i, nCurrentY, pFieldCopy))
                    nCurrentX += i;
                else{
                    fail=true;
                    break;
                }
        }else{
            for(int i=0;i>position; i--)
                if(collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + i, nCurrentY, pFieldCopy))
                    nCurrentX += i;
                else{
                    fail=true;
                    break;
                }
        }
        // Each rotation
        for (int rotation = 0; rotation < 4; rotation++) {
            if(!fail){
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
                thisScore.nLinesCleared=nLinesCleared;
                thisScore.rowTransitions=rowTransitions(pFieldCopy);
                thisScore.yPiecePos=nCurrentY;
                thisScore.aggregateHeight=aggregateHeight(pFieldCopy);

                //rating+=genomes.at(currentGenome).filledSpotCount*thisScore.filledSpotCount;
                //rating+=genomes.at(currentGenome).weightedFilledSpotCount*thisScore.weightedFilledSpotCount;
                rating+=genomes.at(currentGenome).maximumAltitude*thisScore.maximumAltitude;
                rating+=genomes.at(currentGenome).holeCount*thisScore.holeCount;
                //rating+=genomes.at(currentGenome).deepestHole*thisScore.deepestHole;
                //rating+=genomes.at(currentGenome).sumOfAllHoles*thisScore.sumOfAllHoles;
                rating+=genomes.at(currentGenome).nLinesCleared*thisScore.nLinesCleared;
                rating+=genomes.at(currentGenome).rowTransitions*thisScore.rowTransitions;
                rating+=genomes.at(currentGenome).yPiecePos*thisScore.yPiecePos;
                rating+=genomes.at(currentGenome).aggregateHeight*thisScore.aggregateHeight;

                PossibleMoves thisMove;
                thisMove.rating=rating;
                thisMove.rotation=nCurrentRotation;
                thisMove.xPos=nCurrentX;
                thisMove.scores=thisScore;
                vectorPossibleMoves.push_back(thisMove);

                // Restore stuff
                memcpy(pFieldCopy, pField, 12 * 18);
                nCurrentY=oldCurrentY;
            }
            fail=false;
            // Test if piece can be rotated
            for(int i=0;i<rotation; i++)
                if(collisionCheck(nCurrentPiece, nCurrentRotation + i, nCurrentX, nCurrentY, pFieldCopy))
                    nCurrentRotation += i;
                else{
                    fail=true;
                    break;
                }
        }
    }
    return vectorPossibleMoves;
}

// Creates the initial population of genomes, each with random genes.

void createInitialPopulation(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore) {
    uniform_real_distribution<double> range(-4.0, 4.0);
    Genome thisGenome;
    // For a given population size
    for (int i = 0; i < populationSize+1; i++) {
        // Randomly initialize the values that make up a genome
        // These are all weight values that are updated through evolution
        thisGenome.id=range(rng);
        thisGenome.filledSpotCount=range(rng);
        thisGenome.weightedFilledSpotCount=range(rng);
        thisGenome.maximumAltitude=range(rng);
        thisGenome.holeCount=range(rng);
        thisGenome.deepestHole=range(rng);
        thisGenome.sumOfAllHoles=range(rng);
        thisGenome.nLinesCleared=range(rng);
        thisGenome.rowTransitions=range(rng);
        thisGenome.yPiecePos=range(rng);
        genomes.push_back(thisGenome);
    }
    evaluateNextGenome(pField,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece,nScore);
}

void makeNextMove(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation,  int nCurrentPiece, int nScore) {
    movesTaken++;
    if (movesTaken > moveLimit) {
        genomes.at(currentGenome).fitness = nScore;
        //Evaluates the next genome
        evaluateNextGenome(pField,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece,nScore);
    } else {
        //get all the possible moves
        double highestRating=-DBL_MAX,highestMove=0;
        vector<PossibleMoves> possibleMoves = getAllPossibleMoves(pField,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece);
        for (int i = 0; i < possibleMoves.size(); i++) {
            if(possibleMoves.at(i).rating>highestRating) {
                highestRating = possibleMoves.at(i).rating;
                highestMove=i;
            }
        }
        PossibleMoves bestMove = possibleMoves.at(highestMove);
        nCurrentRotation=bestMove.rotation;
        nCurrentX=bestMove.xPos;
    }
}

void evaluateNextGenome(unsigned char *pField, int &nCurrentX, int &nCurrentY, int &nCurrentRotation, int nCurrentPiece, int nScore) {
    // Increment index in genome array
    currentGenome++;
    // If there is none, evolves the population.
    if (currentGenome == genomes.size())
        evolve();
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
   swprintf(&screen[(nFieldHeight + 5) * SCREENWIDTH +  2], 30, L"BEST FITNESS: %8d", genomes.at(0).fitness);
   swprintf(&screen[(nFieldHeight + 6) * SCREENWIDTH +  2], 30, L"WORST FITNESS: %8d", genomes.at(genomes.size()-1).fitness);

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
       noble.pop_back();
    }


    // Kill the worst of the population
    while(genomes.size() > populationSize / 2) {
        genomes.pop_back();
    }

    //create children array
    vector<Genome> children;
    //add the fittest genome to array
    children.push_back(genomes.at(0));
    //add population sized amount of children
    uniform_int_distribution<int> rangeChild(0, genomes.size()-1);
    for(int i=0;i<populationSize;i++) {
        children.push_back(makeChild(genomes.at(rangeChild(rng)), genomes.at(rangeChild(rng))));
        //children.push_back(makeChild(genomes.at(rangeChild(rng)), genomes.at(0)));
    }


    genomes.clear();

    genomes.insert(genomes.end(), children.begin(), children.end());
    genomes.pop_back();
    genomes.push_back(noble.at(0));
}

Genome makeChild(Genome mum, Genome dad) {
    // Init the child given two genomes (its 7 parameters + initial fitness value)
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    uniform_real_distribution<double> rangeCrossingOver(0, 1);

    Genome child;
    double choice;
    int choiceRound;

    //choice=rangeCrossingOver(rng);choiceRound=round(choice);
    //choiceRound ? child.filledSpotCount=mum.filledSpotCount : child.filledSpotCount=dad.filledSpotCount;
    //choice=rangeCrossingOver(rng);choiceRound=round(choice);
    //choiceRound ? child.sumOfAllHoles=mum.sumOfAllHoles : child.sumOfAllHoles=dad.sumOfAllHoles;
    //choice=rangeCrossingOver(rng);choiceRound=round(choice);
    //choiceRound ? child.deepestHole=mum.deepestHole : child.deepestHole=dad.deepestHole;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.maximumAltitude=mum.maximumAltitude : child.maximumAltitude=dad.maximumAltitude;
    //choice=rangeCrossingOver(rng);choiceRound=round(choice);
    //choiceRound ? child.weightedFilledSpotCount=mum.weightedFilledSpotCount : child.weightedFilledSpotCount=dad.weightedFilledSpotCount;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.nLinesCleared=mum.nLinesCleared : child.nLinesCleared=dad.nLinesCleared;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.holeCount=mum.holeCount : child.holeCount=dad.holeCount;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.rowTransitions=mum.rowTransitions : child.rowTransitions=dad.rowTransitions;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.yPiecePos=mum.yPiecePos : child.yPiecePos=dad.yPiecePos;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.aggregateHeight=mum.aggregateHeight : child.aggregateHeight=dad.aggregateHeight;
    child.fitness=-1;

    // We mutate each parameter using our mutationstep
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.filledSpotCount = child.filledSpotCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.sumOfAllHoles = child.sumOfAllHoles + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.deepestHole = child.deepestHole + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.maximumAltitude = child.maximumAltitude + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.weightedFilledSpotCount = child.weightedFilledSpotCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
//    if (rangeCrossingOver(rng) < mutationRate) {
//        child.holeCount = child.holeCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
//    }
    return child;
}
