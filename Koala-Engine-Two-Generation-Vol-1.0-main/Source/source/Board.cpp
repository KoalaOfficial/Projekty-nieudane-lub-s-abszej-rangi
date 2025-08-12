<<<<<<< HEAD
#include <iostream>
#include <vector>
#include <array>
#include "../conAnHea/constants.h"
#include "../conAnHea/GameContextBoardH.h"

extern GameContext gameContext;

void setupBoard(std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
    // Wypełniamy planszę pustymi figurami
    for (auto& chunk : chunks) {
        for (auto& row : chunk) {
            row.fill(EMPTY_PIECE);
        }
    }

    // Ustawianie białych figur
    for (int i = 0; i < 8; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] =
            (i == 0 || i == 7) ? WHITE_ROOK :
            (i == 1 || i == 6) ? WHITE_KNIGHT :
            (i == 2 || i == 5) ? WHITE_BISHOP :
            (i == 3) ? WHITE_QUEEN : WHITE_KING;
    }

    // Białe pionki
    for (int i = 8; i < 16; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] = WHITE_PAWN;
    }

    // Czarne figury
    for (int i = 56; i < 64; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] =
            (i == 56 || i == 63) ? BLACK_ROOK :
            (i == 57 || i == 62) ? BLACK_KNIGHT :
            (i == 58 || i == 61) ? BLACK_BISHOP :
            (i == 59) ? BLACK_QUEEN : BLACK_KING;
    }

    // Czarne pionki
    for (int i = 48; i < 56; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] = BLACK_PAWN;
    }
=======
#include <iostream>
#include <vector>
#include <array>
#include "../conAnHea/constants.h"
#include "../conAnHea/GameContextBoardH.h"

extern GameContext gameContext;

void setupBoard(std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
    // Wypełniamy planszę pustymi figurami
    for (auto& chunk : chunks) {
        for (auto& row : chunk) {
            row.fill(EMPTY_PIECE);
        }
    }

    // Ustawianie białych figur
    for (int i = 0; i < 8; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] =
            (i == 0 || i == 7) ? WHITE_ROOK :
            (i == 1 || i == 6) ? WHITE_KNIGHT :
            (i == 2 || i == 5) ? WHITE_BISHOP :
            (i == 3) ? WHITE_QUEEN : WHITE_KING;
    }

    // Białe pionki
    for (int i = 8; i < 16; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] = WHITE_PAWN;
    }

    // Czarne figury
    for (int i = 56; i < 64; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] =
            (i == 56 || i == 63) ? BLACK_ROOK :
            (i == 57 || i == 62) ? BLACK_KNIGHT :
            (i == 58 || i == 61) ? BLACK_BISHOP :
            (i == 59) ? BLACK_QUEEN : BLACK_KING;
    }

    // Czarne pionki
    for (int i = 48; i < 56; ++i) {
        int chunkIdx = gameContext.fieldProperties[i].chunkIndex;
        int rowInChunk = gameContext.fieldProperties[i].rowInChunk;
        int colInChunk = gameContext.fieldProperties[i].colInChunk;

        chunks[chunkIdx][rowInChunk][colInChunk] = BLACK_PAWN;
    }
>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
}