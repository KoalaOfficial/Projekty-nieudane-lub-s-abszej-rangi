#ifndef CHUNK_FEATURE_EXTRACTOR_H
#define CHUNK_FEATURE_EXTRACTOR_H

#include "../conAnHea/StrategicGrids.h"
#include <vector>
#include <cstdint>

struct ChunkFeatureVector {
    int numPieces;
    int numAttackers;
    int numDefenders;
    int pawnStructures;
    int threats;
    int weaknesses;
};

class ChunkFeatureExtractor {
public:
    ChunkFeatureExtractor(const AttackerGrid& attackers,
                          const DefenderGrid& defenders,
                          const NeutralGrid& neutrals);

    ChunkFeatureVector extractFeatures(int chunkIndex, const uint64_t& whiteBB, const uint64_t& blackBB) const;


private:
    const AttackerGrid& attackerGrid;
    const DefenderGrid& defenderGrid;
    const NeutralGrid& neutralGrid;

    int countPieces(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int detectPawnStructures(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int evaluateThreats(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
    int evaluateWeaknesses(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const;
};

#endif // CHUNK_FEATURE_EXTRACTOR_H