//
// Created by Rafael on 10/10/2019.
//
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <ctime>
#include <algorithm>
#include <thread>
#include <vector>
#include <cstdio>
#include <cwchar>


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

const unsigned int seed = time(0);
mt19937_64 rng(seed);
unsigned char *pField = nullptr;

int bagRandomizer(vector<int> &pool) {
    uniform_int_distribution<int> range(0, pool.size() - 1);
    // Choose a random tetromino from pool
    int pos=range(rng);
    int piece = pool.at(pos);
    pool.erase(pool.begin() + pos);

    if (pool.empty())
        pool = {0, 1, 2, 3, 4, 5, 6};

    return piece;
}

int main() {

    // Game Logic
    bool bKey[4];
    int nCurrentRotation = 0;
    int nCurrentX = (nFieldWidth / 2) - 2;
    int nCurrentY = 0;
    int nSpeed = 20;
    int nSpeedCount = 0;
    bool bForceDown = false;
    bool bRotateHold = true;
    int nScore = 0;
    int nLinesCleared=0;
    vector<int> vLines;
    bool bGameOver = false;

    // Variables for Pool Randomizer
    vector<int> pool{0, 1, 2, 3, 4, 5, 6};
    int nCurrentPiece = bagRandomizer(pool);

    // Create Screen Buffer
    wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
    for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = ' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    tetromino[0].append(L"..X...X...X...X."); // I
    tetromino[1].append(L"..X..XX...X....."); // T
    tetromino[2].append(L".....XX..XX....."); // O
    tetromino[3].append(L"..X..XX..X......"); // S
    tetromino[4].append(L".X...XX...X....."); // Z
    tetromino[5].append(L".X...X...XX....."); // J
    tetromino[6].append(L"..X...X..XX....."); // L

    pField = new unsigned char[nFieldWidth*nFieldHeight]; // Create play field buffer
    for (int x = 0; x < nFieldWidth; x++) // Board Boundary
        for (int y = 0; y < nFieldHeight; y++)
            pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
    while(!bGameOver){
        // Timing =======================
        this_thread::sleep_for(50ms); // Small Step = 1 Game Tick
        nSpeedCount++;
        bForceDown = (nSpeedCount == nSpeed);
        // Input ========================
        for (int k = 0; k < 4; k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26"[k]))) != 0;
        // Game Logic ===================
        // Handle player movement
        nCurrentX += (bKey[0] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY, pField)) ? 1
                                                                                                                    : 0;
        nCurrentX -= (bKey[1] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY, pField)) ? 1
                                                                                                                    : 0;
        nCurrentY += (bKey[2] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1, pField)) ? 1
                                                                                                                    : 0;
        // rotate, but latch to stop wild spinning
        if (bKey[3]){
            nCurrentRotation += (bRotateHold &&
                                 collisionCheck(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY, pField)) ? 1
                                                                                                                    : 0;
            bRotateHold = false;
        }
        else
            bRotateHold = true;
        // Force the piece down the playfield if it's time
        if (bForceDown){
            // Update difficulty every 50 pieces
            nSpeedCount = 0;

            // Test if piece can be moved down
            if (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1, pField))
                nCurrentY++;
            else{
                // lock the piece in place
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                // Check for lines
                for (int py = 0; py < 4; py++)
                    if(nCurrentY + py < nFieldHeight - 1){
                        bool bLine = true;
                        for (int px = 1; px < nFieldWidth - 1; px++)
                            bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                        if (bLine)
                            vLines.push_back(nCurrentY + py);

                    }

                nScore += 25;
                if(!vLines.empty()){
                    nScore += (1 << vLines.size()) * 100;
                    nLinesCleared += vLines.size();
                }
                // Pick New Piece
                nCurrentX = (nFieldWidth / 2) - 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = bagRandomizer(pool);

                // If piece does not fit straight away, game over!
                bGameOver = !collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY, pField);
            }
        }
        // Display ======================
        // Draw Field
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y < nFieldHeight; y++)
                screen[(y + 2)*nScreenWidth + (x + 2)] = " ABCDEFG=#"[pField[y*nFieldWidth + x]];
        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != '.')
                    screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
        // Draw Score
        swprintf(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
        // Draw AI Stuff
        swprintf(&screen[3 * nScreenWidth + nFieldWidth + 6], 30, L"FILLED SPOT COUNT: %8d", filledSpotCount(pField,nFieldWidth,nFieldHeight));
        swprintf(&screen[4 * nScreenWidth + nFieldWidth + 6], 40, L"WEIGHTED FILLED SPOT COUNT: %8d", weightedFilledSpotCount(pField,nFieldWidth,nFieldHeight));
        swprintf(&screen[5 * nScreenWidth + nFieldWidth + 6], 40, L"MAXIMUM ALTITUDE: %8d", maximumAltitude(pField,nFieldWidth,nFieldHeight));
        swprintf(&screen[6 * nScreenWidth + nFieldWidth + 6], 40, L"HOLE COUNT: %8d", holeCount(pField,nFieldWidth,nFieldHeight));
        swprintf(&screen[7 * nScreenWidth + nFieldWidth + 6], 30, L"LINES CLEARED: %8d", nLinesCleared);
        swprintf(&screen[8 * nScreenWidth + nFieldWidth + 6], 30, L"DEEPEST HOLE: %8d", deepestHole(pField,nFieldWidth,nFieldHeight));
        swprintf(&screen[9 * nScreenWidth + nFieldWidth + 6], 30, L"SUM OF ALL HOLES: %8d", sumOfAllHoles(pField,nFieldWidth,nFieldHeight));
        // Clear line
        if (!vLines.empty()){
            for (auto &v : vLines)
                for (int px = 1; px < nFieldWidth - 1; px++){
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[px] = 0;
                }

            vLines.clear();
        }
        // Display Frame
        WriteConsoleOutputCharacter(hConsole, (screen), nScreenWidth * nScreenHeight, {0, 0 }, &dwBytesWritten);
    }
    CloseHandle(hConsole);
    cout << "Game Over!! Score:" << nScore << endl;
    system("pause");
    return 0;
}