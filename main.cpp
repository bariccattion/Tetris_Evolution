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
#include "ai.h"
#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

using namespace std;

int nScreenWidth = 120;			// Console Screen Size X (columns)
int nScreenHeight = 50;			// Console Screen Size Y (rows)

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;


int rotate(int px, int py, int r){
    //Rotation matrix in 1d vector format
    int pi = 0;
    switch (r % 4){
        case 0: //0
            pi = py * 4 + px;
            break;

        case 1: //90
            pi = 12 + py - (px * 4);
            break;

        case 2: //180
            pi = 15 - (py * 4) - px;
            break;

        case 3: //270
            pi = 3 - py + (px * 4);
            break;
    }
    return pi;
}

bool collisionCheck(int nTetromino, int nRotation, int nPosX, int nPosY){
    // All Field cells >0 are occupied
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++){
            // Get index into piece
            int pi = rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            // Check that test is in bounds.
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth){
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight){
                    // In Bounds so do collision check
                    if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
                        return false; // fail on first hit
                }
            }
        }

    return true;
}

int tgm3Randomizer(vector<int> pool) {
    const unsigned int seed = time(0);
    mt19937_64 rng(seed);
    uniform_int_distribution<int> range(0, 34);
    int piece;
    map<int,int> dup;

    // Choose a random tetromino from pool
    int pos=range(rng);
    piece = pool.at(pos);
    pool.erase(pool.begin()+pos);

    // Count how many times each element is repeated in pool
    for_each( pool.begin(), pool.end(), [&dup]( int val ){ dup[val]++; } );

    int mostRepeated=0,blockRepeated=0;
    for(int i=0;i<7;i++)
        if(dup.at(i)>mostRepeated){
            mostRepeated=dup.at(i);
            blockRepeated=i;// blockRepeated is the most repeated tetromino in pool, so it`s the most droughted piece;
        }

    pool.push_back(blockRepeated);
    return piece;
}

int main() {

    // Game Logic
    bool bKey[4];
    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;
    int nSpeed = 20;
    int nSpeedCount = 0;
    bool bForceDown = false;
    bool bRotateHold = true;
    int nPieceCount = 0;
    int nScore = 0;
    int nLinesCleared=0;
    vector<int> vLines;
    bool bGameOver = false;

    // Variables for TGM3 Random
    vector<int> pieces{0,1,2,3,4,5,6};
    vector<int> pool;
    for(int i=0;i<5;i++){
        pool.insert(pool.end(),pieces.begin(),pieces.end());
    }

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
        nCurrentX += (bKey[0] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        nCurrentX -= (bKey[1] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
        nCurrentY += (bKey[2] && collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
        // rotate, but latch to stop wild spinning
        if (bKey[3]){
            nCurrentRotation += (bRotateHold && collisionCheck(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = false;
        }
        else
            bRotateHold = true;
        // Force the piece down the playfield if it's time
        if (bForceDown){
            // Update difficulty every 50 pieces
            nSpeedCount = 0;
            nPieceCount++;
            if (nPieceCount % 50 == 0)
                if (nSpeed >= 10) nSpeed--;

            // Test if piece can be moved down
            if (collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++; //
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
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = tgm3Randomizer(pool);

                // If piece does not fit straight away, game over!
                bGameOver = !collisionCheck(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
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