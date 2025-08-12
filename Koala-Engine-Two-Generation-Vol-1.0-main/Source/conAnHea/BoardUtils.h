<<<<<<< HEAD
#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <random>
#include <utility>
#include "../conAnHea/constants.h"
#include "../conAnHea/GameContextBoardH.h"

extern GameContext gameContext;

class BoardUtils {
private:
    std::array<std::array<std::array<uint64_t, BOARD_SIZE * BOARD_SIZE>, NUM_PIECE_TYPES>, NUM_COLORS> zobristTable;
    uint64_t currentHash;
    uint64_t whiteBitboard;
    uint64_t blackBitboard;

public:
    BoardUtils() : currentHash(0), whiteBitboard(0), blackBitboard(0) {
        initializeZobristTable();
    }

    void initializeZobristTable() {
        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist;

        for (int color = 0; color < NUM_COLORS; ++color) {
            for (int type = 0; type < NUM_PIECE_TYPES; ++type) {
                for (int position = 0; position < BOARD_SIZE * BOARD_SIZE; ++position) {
                    zobristTable[color][type][position] = dist(rng);
                }
            }
        }
    }

    void initialize(const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
        currentHash = 0;
        whiteBitboard = 0;
        blackBitboard = 0;

        for (int chunkIndex = 0; chunkIndex < NUM_CHUNKS; ++chunkIndex) {
            for (int row = 0; row < CHUNK_SIZE; ++row) {
                for (int col = 0; col < CHUNK_SIZE; ++col) {
                    const Piece& piece = chunks[chunkIndex][row][col];
                    if (piece.type != PieceType::EMPTY) {
                        int position = chunkIndex * CHUNK_SIZE * CHUNK_SIZE + row * CHUNK_SIZE + col;

                        currentHash ^= zobristTable[static_cast<int>(piece.color)][static_cast<int>(piece.type)][position];

                        if (piece.color == Color::WHITE) {
                            whiteBitboard |= (1ULL << position);
                        } else {
                            blackBitboard |= (1ULL << position);
                        }
                    }
                }
            }
        }
    }

    uint64_t getCurrentHash() const { return currentHash; }
    uint64_t getWhiteBitboard() const { return whiteBitboard; }
    uint64_t getBlackBitboard() const { return blackBitboard; }
    const auto& getZobristTable() const { return zobristTable; }
    std::pair<uint64_t, uint64_t> getBothBitboards() const { return {whiteBitboard, blackBitboard}; }

    void setWhiteBitboard(uint64_t bitboard) { whiteBitboard = bitboard; }
    void setBlackBitboard(uint64_t bitboard) { blackBitboard = bitboard; }
    void setCurrentHash(uint64_t hash) { currentHash = hash; }
};

=======
#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <random>
#include <utility>
#include "../conAnHea/constants.h"
#include "../conAnHea/GameContextBoardH.h"

extern GameContext gameContext;

class BoardUtils {
private:
    std::array<std::array<std::array<uint64_t, BOARD_SIZE * BOARD_SIZE>, NUM_PIECE_TYPES>, NUM_COLORS> zobristTable;
    uint64_t currentHash;
    uint64_t whiteBitboard;
    uint64_t blackBitboard;

public:
    BoardUtils() : currentHash(0), whiteBitboard(0), blackBitboard(0) {
        initializeZobristTable();
    }

    void initializeZobristTable() {
        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist;

        for (int color = 0; color < NUM_COLORS; ++color) {
            for (int type = 0; type < NUM_PIECE_TYPES; ++type) {
                for (int position = 0; position < BOARD_SIZE * BOARD_SIZE; ++position) {
                    zobristTable[color][type][position] = dist(rng);
                }
            }
        }
    }

    void initialize(const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
        currentHash = 0;
        whiteBitboard = 0;
        blackBitboard = 0;

        for (int chunkIndex = 0; chunkIndex < NUM_CHUNKS; ++chunkIndex) {
            for (int row = 0; row < CHUNK_SIZE; ++row) {
                for (int col = 0; col < CHUNK_SIZE; ++col) {
                    const Piece& piece = chunks[chunkIndex][row][col];
                    if (piece.type != PieceType::EMPTY) {
                        int position = chunkIndex * CHUNK_SIZE * CHUNK_SIZE + row * CHUNK_SIZE + col;

                        currentHash ^= zobristTable[static_cast<int>(piece.color)][static_cast<int>(piece.type)][position];

                        if (piece.color == Color::WHITE) {
                            whiteBitboard |= (1ULL << position);
                        } else {
                            blackBitboard |= (1ULL << position);
                        }
                    }
                }
            }
        }
    }

    uint64_t getCurrentHash() const { return currentHash; }
    uint64_t getWhiteBitboard() const { return whiteBitboard; }
    uint64_t getBlackBitboard() const { return blackBitboard; }
    const auto& getZobristTable() const { return zobristTable; }
    std::pair<uint64_t, uint64_t> getBothBitboards() const { return {whiteBitboard, blackBitboard}; }

    void setWhiteBitboard(uint64_t bitboard) { whiteBitboard = bitboard; }
    void setBlackBitboard(uint64_t bitboard) { blackBitboard = bitboard; }
    void setCurrentHash(uint64_t hash) { currentHash = hash; }
};

>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
#endif // BOARD_UTILS_H