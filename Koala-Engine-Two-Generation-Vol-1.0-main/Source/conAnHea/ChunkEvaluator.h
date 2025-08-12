#ifndef CHUNKEVALUATOR_H
#define CHUNKEVALUATOR_H


#include <vector>
#include <memory>
#include <cstdint>
#include <array>
#include "../conAnHea/ChunkFeatureExtractor.h"
#include "../conAnHea/ChunkMaskGenerator.h"
#include "../conAnHea/build_chunks.h"
#include "../conAnHea/GameContextBoardH.h"

class IEvaluator {
public:
    virtual ~IEvaluator() = default;
    virtual float evaluateChunk(const Chunk& chunk, const BoardSnapshot& snapshot) const = 0;
    virtual std::vector<float> evaluateChunks(const std::vector<Chunk>& chunks, const BoardSnapshot& snapshot) const = 0;
};

class ChunkEvaluator : public IEvaluator {
public:
    ChunkEvaluator(std::unique_ptr<ChunkFeatureExtractor> extractor,
                   std::unique_ptr<ChunkMaskGenerator> maskGen);

    ChunkEvaluator(const AttackerGrid& attackerGrid,
                   const DefenderGrid& defenderGrid,
                   const NeutralGrid& neutralGrid);

    float evaluateChunk(const Chunk& chunk, const BoardSnapshot& snapshot) const override;
    std::vector<float> evaluateChunks(const std::vector<Chunk>& chunks, const BoardSnapshot& snapshot) const override;

    // Helper for compatibility: evaluateChunks for old type
    std::vector<float> evaluateChunks(const std::vector<std::array<std::array<Piece, 2>, 2>>& arrays, const BoardSnapshot& snapshot) const;

private:
    std::unique_ptr<ChunkFeatureExtractor> featureExtractor;
    std::unique_ptr<ChunkMaskGenerator> maskGenerator;
    std::array<uint64_t, NUM_CHUNKS> splitBitboardToChunks(uint64_t bitboard) const;
};

#endif // CHUNKEVALUATOR_H