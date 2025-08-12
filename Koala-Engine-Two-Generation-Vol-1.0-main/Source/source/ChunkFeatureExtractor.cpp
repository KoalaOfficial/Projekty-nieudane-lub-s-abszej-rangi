#include "../conAnHea/ChunkFeatureExtractor.h"

ChunkFeatureExtractor::ChunkFeatureExtractor(const AttackerGrid& attackers,
                                             const DefenderGrid& defenders,
                                             const NeutralGrid& neutrals)
    : attackerGrid(attackers), defenderGrid(defenders), neutralGrid(neutrals) {}

ChunkFeatureVector ChunkFeatureExtractor::extractFeatures(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const {
    ChunkFeatureVector features;
    features.numPieces = countPieces(chunkIndex, whiteBitboard, blackBitboard);
    features.numAttackers = attackerGrid.countAttackers(chunkIndex, whiteBitboard, blackBitboard);
    features.numDefenders = defenderGrid.countDefenders(chunkIndex, whiteBitboard, blackBitboard);
    features.pawnStructures = detectPawnStructures(chunkIndex, whiteBitboard, blackBitboard);
    features.threats = evaluateThreats(chunkIndex, whiteBitboard, blackBitboard);
    features.weaknesses = evaluateWeaknesses(chunkIndex, whiteBitboard, blackBitboard);
    return features;
}

int ChunkFeatureExtractor::countPieces(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const {
    return attackerGrid.countPieces(chunkIndex, whiteBitboard, blackBitboard) +
           defenderGrid.countPieces(chunkIndex, whiteBitboard, blackBitboard) +
           neutralGrid.countPieces(chunkIndex, whiteBitboard, blackBitboard);
}

int ChunkFeatureExtractor::detectPawnStructures(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const {
    return neutralGrid.detectPawnChains(chunkIndex, whiteBitboard, blackBitboard);
}

int ChunkFeatureExtractor::evaluateThreats(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const {
    return attackerGrid.evaluateThreats(chunkIndex, whiteBitboard, blackBitboard);
}

int ChunkFeatureExtractor::evaluateWeaknesses(int chunkIndex, const uint64_t& whiteBitboard, const uint64_t& blackBitboard) const {
    return defenderGrid.evaluateWeaknesses(chunkIndex, whiteBitboard, blackBitboard);
}
