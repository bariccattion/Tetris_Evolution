//
// Created by Rafael on 10/10/2019.
//

#include "ai.h"

const unsigned int seed = time(0);
mt19937_64 rng(seed);

double mutationRate = 0.05;
double mutationStep = 0.2;
int moveLimit = 500;
int movesTaken = 0;
int populationSize = 10;
int currentGenome = 0;
int generation=0;

vector<Genome> genomes;
vector<Genome> noble;

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

vector <PossibleMoves> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int nCurrentPiece) {

    wstring tetromino[7];
    tetromino[0].append(L"..X...X...X...X."); // I
    tetromino[1].append(L"..X..XX...X....."); // T
    tetromino[2].append(L".....XX..XX....."); // O
    tetromino[3].append(L"..X..XX..X......"); // S
    tetromino[4].append(L".X...XX...X....."); // Z
    tetromino[5].append(L".X...X...XX....."); // J
    tetromino[6].append(L"..X...X..XX....."); // L

    unsigned char *pFieldCopy = (unsigned char *) malloc(12 * 18);
    int nCurrentRotation = 0, oldCurrentY;
    vector<PossibleMoves> vectorPossibleMoves;
    memcpy(pFieldCopy, pField, 12 * 18);


    // Each position
    for (int position = -3; position < 3; position++) {
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

void createInitialPopulation(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore) {
    uniform_real_distribution<double> range(-1.0, 1.0);
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
    evaluateNextGenome(pField,nCurrentX,nCurrentY,nCurrentPiece,nScore);
}

void makeNextMove(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore) {

    wstring tetromino[7];
    tetromino[0].append(L"..X...X...X...X."); // I
    tetromino[1].append(L"..X..XX...X....."); // T
    tetromino[2].append(L".....XX..XX....."); // O
    tetromino[3].append(L"..X..XX..X......"); // S
    tetromino[4].append(L".X...XX...X....."); // Z
    tetromino[5].append(L".X...X...XX....."); // J
    tetromino[6].append(L"..X...X..XX....."); // L

    movesTaken++;
    //if its over the limit of moves
    if (movesTaken > moveLimit) {
        //update this genomes fitness value using the game score
        genomes.at(currentGenome).fitness = nScore;
        //and evaluates the next genome
        evaluateNextGenome(pField,nCurrentX,nCurrentY,nCurrentPiece, nScore);
    } else {
        //get all the possible moves
        double highestRating=-DBL_MAX,highestMove=0;
        vector<PossibleMoves> possibleMoves = getAllPossibleMoves(pField,nCurrentX,nCurrentY,nCurrentPiece);
        for (int i = 0; i < possibleMoves.size(); i++) {
            if(possibleMoves.at(i).rating>highestRating) {
                highestRating = possibleMoves.at(i).rating;
                highestMove=i;
            }
        }
        PossibleMoves bestMove = possibleMoves.at(highestMove);
        int nCurrentRotation=0;
        for (int rotations = 0; rotations < bestMove.rotation; rotations++) {
            nCurrentRotation += (collisionCheck(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY, pField)) ? 1 : 0;
        }
        if (bestMove.xPos < 0) {
            for (int lefts = 0; lefts > bestMove.xPos; lefts--) {
                nCurrentX -= (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY, pField)) ? 1 : 0;
            }
        } else if (bestMove.xPos > 0) {
            for (int rights = 0; rights < bestMove.xPos; rights++) {
                nCurrentX += (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY, pField)) ? 1 : 0;
            }
        }
//        for (int px = 0; px < 4; px++)
//            for (int py = 0; py < 4; py++)
//                if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
//                    pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
    }
}

void evaluateNextGenome(unsigned char *pField, int &nCurrentX, int &nCurrentY, int nCurrentPiece, int nScore) {
    //increment index in genome array
    currentGenome++;
    //If there is none, evolves the population.
    if (currentGenome == genomes.size()) {
        //evolve();
    }
    //reset moves taken
    movesTaken = 0;
    //and make the next move
    makeNextMove(pField,nCurrentX,nCurrentY,nCurrentPiece,nScore);
}

void evolve() {
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    currentGenome = 0;
    generation++;
    resetGame();

    // Sort genomes by fitness
    for (int i = 0; i < genomes.size(); i++)
        for (int j = 0; j < genomes.size()-i; j++)
            if (genomes.at(j).fitness > genomes.at(j+1).fitness){
                Genome temp = genomes.at(j);
                genomes.at(j) = genomes.at(j+1);
                genomes.at(j+1) = temp;
            }

    // Add a copy of the fittest genome to the nobles list and sort it
    noble.push_back(genomes.at(0));
    for (int i = 0; i < noble.size(); i++)
        for (int j = 0; j < noble.size()-i; j++)
            if(noble.size()>1)
                if (noble.at(j).fitness > noble.at(j+1).fitness){
                    Genome temp = noble.at(j);
                    noble.at(j) = noble.at(j+1);
                    noble.at(j+1) = temp;
                }

    // Kill the worst of the population
    while(genomes.size() > populationSize / 2) {
        genomes.pop_back();
    }

    // Sum of the fitness for each genome
    int totalFitness = 0;
    for (int i = 0; i < genomes.size(); i++) {
        totalFitness += genomes.at(i).fitness;
    }

    //create children array
    vector<Genome> children;
    //add the fittest genome to array
    children.push_back(genomes.at(0));
    //add population sized amount of children
    while (children.size() < populationSize/2) {

        uniform_int_distribution<int> rangeChild(0, genomes.size()-1);
        children.push_back(makeChild(genomes.at(rangeChild(rng)), genomes.at(rangeChild(rng))));
        children.push_back(makeChild(genomes.at(rangeChild(rng)), noble.at(0)));
    }
    //create new genome array
    genomes.clear();
    //to store all the children in
    genomes.insert(genomes.end(), children.begin(), children.end());
}

Genome makeChild(Genome mum, Genome dad) {
    // Init the child given two genomes (its 7 parameters + initial fitness value)
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    uniform_real_distribution<double> rangeCrossingOver(0, 1);

    Genome child;
    double choice;
    int choiceRound;

    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.filledSpotCount=mum.filledSpotCount : child.filledSpotCount=dad.filledSpotCount;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.sumOfAllHoles=mum.sumOfAllHoles : child.sumOfAllHoles=dad.sumOfAllHoles;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.deepestHole=mum.deepestHole : child.deepestHole=dad.deepestHole;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.maximumAltitude=mum.maximumAltitude : child.maximumAltitude=dad.maximumAltitude;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.weightedFilledSpotCount=mum.weightedFilledSpotCount : child.weightedFilledSpotCount=dad.weightedFilledSpotCount;
    choice=rangeCrossingOver(rng);choiceRound=round(choice);
    choiceRound ? child.holeCount=mum.holeCount : child.holeCount=dad.holeCount;
    child.fitness=-1;

    // We mutate each parameter using our mutationstep
    if (rangeCrossingOver(rng) < mutationRate) {
        child.filledSpotCount = child.filledSpotCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        child.sumOfAllHoles = child.sumOfAllHoles + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        child.deepestHole = child.deepestHole + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        child.maximumAltitude = child.maximumAltitude + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        child.weightedFilledSpotCount = child.weightedFilledSpotCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    if (rangeCrossingOver(rng) < mutationRate) {
        child.holeCount = child.holeCount + rangeCrossingOver(rng) * mutationStep * 2 - mutationStep;
    }
    return child;
}
