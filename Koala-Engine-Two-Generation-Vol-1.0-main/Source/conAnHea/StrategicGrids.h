#ifndef STRATEGIC_GRIDS_H
#define STRATEGIC_GRIDS_H

#include <vector>
#include <cstdint>
#include <array>
#include "constants.h"

// Forward declaration, jeśli potrzebujesz ChunkFeatureVector
struct ChunkFeatureVector;

uint64_t getChunkMask(int chunkIndex);
std::vector<float> toVector(const ChunkFeatureVector& cfv);

struct AttackerGrid {
    std::array<int, BOARD_SIZE> influence = {};
    int countAttackers(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int countPieces(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int evaluateThreats(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
};

struct DefenderGrid {
    std::array<int, BOARD_SIZE> influence = {};
    int countDefenders(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int countPieces(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int evaluateWeaknesses(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
};

struct NeutralGrid {
    std::array<int, BOARD_SIZE> influence = {};
    int countPieces(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int detectPawnChains(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
};

#endif // STRATEGIC_GRIDS_H