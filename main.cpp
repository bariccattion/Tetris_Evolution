//
// Created by Rafael on 10/10/2019.
//
#include <list>
#include <vector>
#include <cstdio>

#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>
#include "ai.h"
#include "functions.h"

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

int main() {
    // Game Logic
    bool bKey[5];
    int nCurrentRotation = 0;
    int nCurrentX = (nFieldWidth / 2) - 2;
    int nCurrentY = 0;
    int nPieces=0;
    vector<int> vLines;
    int nScore=0;
    int bestNPieces=0,bestScore=0, bestNLines=0;

    // Variables for Pool Randomizer
    vector<int> pool{0, 1, 2, 3, 4, 5, 6};
    int nCurrentPiece = bagRandomizer(pool);

    // Create Screen Buffer
    wchar_t *screen = new wchar_t[SCREENWIDTH*SCREENHEIGHT];
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) screen[i] = ' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    SMALL_RECT windowSize = {0, 0, static_cast<SHORT>(SCREENWIDTH), static_cast<SHORT>(SCREENHEIGHT)};
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // Tetrominos
    tetromino[0].append(L"..X...X...X...X."); // I
    tetromino[1].append(L"..X..XX...X....."); // T
    tetromino[2].append(L".....XX..XX....."); // O
    tetromino[3].append(L"..X..XX..X......"); // S
    tetromino[4].append(L".X...XX...X....."); // Z
    tetromino[5].append(L".X...X...XX....."); // J
    tetromino[6].append(L"..X...X..XX....."); // L

    unsigned char *pFieldLocal = nullptr;
    pFieldLocal = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer
    for (int x = 0; x < nFieldWidth; x++) // Board Boundary
        for (int y = 0; y < nFieldHeight; y++)
            pFieldLocal[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    // AI Startup
    createInitialPopulation(pFieldLocal,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece,nScore);

    while(true){
        // Input
        for (int k = 0; k < 5; k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\xA0"[k]))) != 0;

        // AI Do move
        makeNextMove(pFieldLocal,nCurrentX,nCurrentY,nCurrentRotation,nCurrentPiece,nScore);

        // Put piece in field
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
                    pFieldLocal[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

        // Check for lines
        for (int py = 0; py < 4; py++)
            if(nCurrentY + py < nFieldHeight - 1){
                bool bLine = true;
                for (int px = 1; px < nFieldWidth - 1; px++)
                    bLine &= (pFieldLocal[(nCurrentY + py) * nFieldWidth + px]) != 0;
                if (bLine)
                    vLines.push_back(nCurrentY + py);
            }

        // Clear line and add score for cleaning lines
        if(!vLines.empty()){
            nScore += (1 << vLines.size()) * 100;
            nLinesCleared += vLines.size();

            for (auto &v : vLines)
                for (int px = 1; px < nFieldWidth - 1; px++){
                    for (int py = v; py > 0; py--)
                        pFieldLocal[py * nFieldWidth + px] = pFieldLocal[(py - 1) * nFieldWidth + px];
                    pFieldLocal[px] = 0;
                }
            vLines.clear();
        }
        // Score for placing tetromino
        nScore += 25;

        // Pick New Piece
        nCurrentX = (nFieldWidth / 2) - 2;
        nCurrentY = 0;
        nCurrentRotation = 0;
        nCurrentPiece = bagRandomizer(pool);

        // If piece does not fit straight away, game over!
        bGameOver = !collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY, pFieldLocal);
        if(bGameOver){
            // Make new field
            delete[] pFieldLocal;
            pFieldLocal = new unsigned char[nFieldWidth * nFieldHeight];
            for (int x = 0; x < nFieldWidth; x++)
                for (int y = 0; y < nFieldHeight; y++)
                    pFieldLocal[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

            nCurrentRotation = 0;
            nCurrentX = (nFieldWidth / 2) - 1;
            vLines.clear();

            if(nScore>bestScore)
                bestScore=nScore;
            if(nPieces>bestNPieces)
                bestNPieces=nPieces;
            if(nLinesCleared>bestNLines)
                bestNLines=nLinesCleared;

            //Evaluate next genome and play again;
            evaluateNext(pFieldLocal, nCurrentX, nCurrentY, nCurrentRotation, nCurrentPiece, nScore);

            nCurrentY = 0;
            nLinesCleared=0;
            nScore=0;
            nPieces=0;

        }else{
            nPieces++;
        }

        if(bKey[4]){
            // Display ======================
            // Draw Field
            for (int x = 0; x < nFieldWidth; x++)
                for (int y = 0; y < nFieldHeight; y++)
                    screen[(y + 2)*SCREENWIDTH + (x + 2)] = " ABCDEFG=#"[pFieldLocal[y * nFieldWidth + x]];

            // Draw Score
            swprintf(&screen[1 * SCREENWIDTH + 2], 16, L"SCORE: %8d", nScore);

            // Draw AI Stuff
            swprintf(&screen[2 * SCREENWIDTH + nFieldWidth + 6], 40, L"MAXIMUM ALTITUDE: %8d", maximumAltitude(pFieldLocal));
            swprintf(&screen[3 * SCREENWIDTH + nFieldWidth + 6], 40, L"HOLE COUNT: %8d", holeCount(pFieldLocal));
            swprintf(&screen[4 * SCREENWIDTH + nFieldWidth + 6], 50, L"CONNECTED HOLES COUNT : %8d", connectedHolesCount(pFieldLocal));
            swprintf(&screen[5 * SCREENWIDTH + nFieldWidth + 6], 30, L"MAXIMUM WELL DEPTH: %8d", maximumWellDepth(pFieldLocal));
            swprintf(&screen[6 * SCREENWIDTH + nFieldWidth + 6], 30, L"FILLED SPOT COUNT: %8d", filledSpotCount(pFieldLocal));
            swprintf(&screen[7 * SCREENWIDTH + nFieldWidth + 6], 40, L"WEIGHTED FILLED SPOT COUNT: %8d", weightedFilledSpotCount(pFieldLocal));
            swprintf(&screen[8 * SCREENWIDTH + nFieldWidth + 6], 30, L"N LINES CLEARED: %8d", nLinesCleared);
            swprintf(&screen[9 * SCREENWIDTH + nFieldWidth + 6], 30, L"ROW TRANSITIONS: %8d", rowTransitions(pFieldLocal));

            swprintf(&screen[11 * SCREENWIDTH + nFieldWidth + 6], 50, L"CURRENT GENOME:");
            swprintf(&screen[12 * SCREENWIDTH + nFieldWidth + 6], 50, L"MAXIMUM ALTITUDE: %8f", genomes.at(currentGenome).maximumAltitude);
            swprintf(&screen[13 * SCREENWIDTH + nFieldWidth + 6], 50, L"HOLECOUNT : %8f", genomes.at(currentGenome).holeCount);
            swprintf(&screen[14 * SCREENWIDTH + nFieldWidth + 6], 50, L"CONNECTED HOLES COUNT: %8f", genomes.at(currentGenome).connectedHolesCount);
            swprintf(&screen[15 * SCREENWIDTH + nFieldWidth + 6], 50, L"MAXIMUM WELL DEPTH: %8f", genomes.at(currentGenome).maximumWellDepth);
            swprintf(&screen[16 * SCREENWIDTH + nFieldWidth + 6], 50, L"FILLED SPOT COUNT: %8f", genomes.at(currentGenome).filledSpotCount);
            swprintf(&screen[17 * SCREENWIDTH + nFieldWidth + 6], 50, L"WEIGHTED FILLED SPOT COUNT: %8f", genomes.at(currentGenome).weightedFilledSpotCount);
            swprintf(&screen[18 * SCREENWIDTH + nFieldWidth + 6], 50, L"N LINES CLEARED: %8f", genomes.at(currentGenome).nLinesCleared);
            swprintf(&screen[19 * SCREENWIDTH + nFieldWidth + 6], 50, L"ROW TRANSITIONS: %8f", genomes.at(currentGenome).rowTransitions);

            swprintf(&screen[(nFieldHeight + 2) * SCREENWIDTH +  2], 30, L"GENERATION: %8d", generation);
            swprintf(&screen[(nFieldHeight + 3) * SCREENWIDTH +  2], 30, L"CURRENT GENOME: %8d", currentGenome);

            swprintf(&screen[(nFieldHeight + 6) * SCREENWIDTH +  2], 60, L"MOST LINES: %8d", bestNLines);
            swprintf(&screen[(nFieldHeight + 7) * SCREENWIDTH +  2], 60, L"BEST SCORE FROM ALL: %8d", bestScore);
            swprintf(&screen[(nFieldHeight + 8) * SCREENWIDTH +  2], 60, L"MOST PIECES: %8d", bestNPieces);

            // Display Frame
            WriteConsoleOutputCharacter(hConsole, (screen), SCREENWIDTH * SCREENHEIGHT, {0, 0 }, &dwBytesWritten);
        }
    }
}