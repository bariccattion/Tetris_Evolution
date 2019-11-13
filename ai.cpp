//
// Created by Rafael on 10/10/2019.
//

#include "ai.h"

int filledSpotCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int filledSpots=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0)
                filledSpots++;
    }
    return filledSpots;
}

int weightedFilledSpotCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int filledSpots=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0)
                filledSpots=filledSpots+1*(nFieldHeight-py-1);
    }
    return filledSpots;
}

int maximumAltitude(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int maxBlockPos=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 0; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]!=0 && (nFieldHeight-py-1)>maxBlockPos)
                maxBlockPos=(nFieldHeight-py-1);
    }
    return maxBlockPos;
}

int holeCount(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int nCount=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0)
                nCount++;
    }
    return nCount;
}

int deepestHole(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int deepestHolePos=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0 && pField[(py) * nFieldWidth + px+1]!=0 && pField[(py) * nFieldWidth + px-1]!=0)
                deepestHolePos=(nFieldHeight-py-1);
    }
    return deepestHolePos;
}

int sumOfAllHoles(const unsigned char *pField, int nFieldWidth, int nFieldHeight) {
    int sumOfAllHoles=0;
    for (int px = 1; px < nFieldWidth - 1; px++){
        for (int py = 1; py < nFieldHeight - 1; py++)
            if(pField[py * nFieldWidth + px]==0 && pField[(py-1) * nFieldWidth + px]!=0 && pField[(py) * nFieldWidth + px+1]!=0 && pField[(py) * nFieldWidth + px-1]!=0)
                sumOfAllHoles+=(nFieldHeight-py-1);
    }
    return sumOfAllHoles;
}

vector<int> getAllPossibleMoves(unsigned char *pField, int nCurrentX, int nCurrentY, int score, int nLinesCleared,
                                int nCurrentPiece) {
    unsigned char *pFieldCopy = (unsigned char *) malloc(12 * 18);
    memcpy(pFieldCopy, pField, 12 * 18);
    int nCurrentRotation = 0;

    // Each position
    for (int position = -5; position < 5; position++) {
        //Do stuff before moving

        // Test if piece can be moved
        nCurrentX += (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY, pFieldCopy)) ? 1 : 0;

        // Each rotation
        for (int rotation = 0; rotation < 4; rotation++) {
            //Do stuff before rotating


            // Test if piece can be rotated
            nCurrentRotation += (collisionCheck(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY, pFieldCopy))
                                ? 1 : 0;
        }
    }
}
