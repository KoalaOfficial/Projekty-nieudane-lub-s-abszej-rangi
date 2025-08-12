#include "../conAnHea/StrategicGrids.h"
#include "../conAnHea/BoardUtils.h"
#include "constants.h"
#include "../conAnHea/ChunkFeatureExtractor.h"  // <-- DODAJ TEN INCLUDE, jeśli tu jest definicja!

#include <bitset>

// Helper: get bitmask for a given chunk index (assuming 8x8 board, 8 chunks)
uint64_t getChunkMask(int chunkIndex) {
    // Assuming chunkIndex 0-7 corresponds to 8 horizontal chunks (rows)
    return 0xFFULL << (chunkIndex * 8);
}

// Funkcja zamieniająca ChunkFeatureVector na std::vector<float>
std::vector<float> toVector(const ChunkFeatureVector& cfv) {
    return {
        static_cast<float>(cfv.numPieces),
        static_cast<float>(cfv.numAttackers),
        static_cast<float>(cfv.numDefenders),
        static_cast<float>(cfv.pawnStructures),
        static_cast<float>(cfv.threats),
        static_cast<float>(cfv.weaknesses)
    };
}
// Count set bits in a masked region
int countBitsInChunk(uint64_t bitboard, int chunkIndex){
    return std::bitset<64>(bitboard & getChunkMask(chunkIndex)).count();
}

// ---------------- AttackerGrid ----------------

int AttackerGrid::countAttackers(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const{
    // Attackers = enemy pieces in chunk
    return countBitsInChunk(blackBB, chunkIndex);
}

int AttackerGrid::countPieces(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const{
    return countBitsInChunk(whiteBB | blackBB, chunkIndex);
}

int AttackerGrid::evaluateThreats(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB)const {
    int attackers = countAttackers(chunkIndex, whiteBB, blackBB);
    int total = countPieces(chunkIndex, whiteBB, blackBB);
    return attackers * 10 - (total - attackers) * 2;
}

// ---------------- DefenderGrid ----------------

int DefenderGrid::countDefenders(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const {
    return countBitsInChunk(whiteBB, chunkIndex);
}

int DefenderGrid::countPieces(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB)const {
    return countBitsInChunk(whiteBB | blackBB, chunkIndex);
}

int DefenderGrid::evaluateWeaknesses(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const{
    int defenders = countDefenders(chunkIndex, whiteBB, blackBB);
    int total = countPieces(chunkIndex, whiteBB, blackBB);
    return (total - defenders) * 5 - defenders * 2;
}

// ---------------- NeutralGrid ----------------

int NeutralGrid::countPieces(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const{
    return countBitsInChunk(whiteBB | blackBB, chunkIndex);
}

int NeutralGrid::detectPawnChains(int chunkIndex, const uint64_t& whitePawns, const uint64_t& blackPawns)const {
    // Simplified: count adjacent pawns in chunk
    uint64_t chunkMask = getChunkMask(chunkIndex);
    uint64_t white = whitePawns & chunkMask;
    uint64_t black = blackPawns & chunkMask;

    int chains = 0;
    for (int i = 0; i < 7; ++i) {
        if ((white >> (chunkIndex * 8 + i)) & 1 && (white >> (chunkIndex * 8 + i + 1)) & 1)
            chains++;
        if ((black >> (chunkIndex * 8 + i)) & 1 && (black >> (chunkIndex * 8 + i + 1)) & 1)
            chains++;
    }
    return chains;
}