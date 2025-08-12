#include "../conAnHea/ChunkEvaluator.h"
#include "../conAnHea/build_chunks.h"
#include <cassert>
#include <algorithm>

// Konstruktor przez unique_ptr
ChunkEvaluator::ChunkEvaluator(std::unique_ptr<ChunkFeatureExtractor> extractor,
                               std::unique_ptr<ChunkMaskGenerator> maskGen)
    : featureExtractor(std::move(extractor)), maskGenerator(std::move(maskGen)) {}

// Konstruktor z siatkami 
ChunkEvaluator::ChunkEvaluator(const AttackerGrid& attackerGrid,
                               const DefenderGrid& defenderGrid,
                               const NeutralGrid& neutralGrid)
    : featureExtractor(std::make_unique<ChunkFeatureExtractor>(attackerGrid, defenderGrid, neutralGrid)),
      maskGenerator(std::make_unique<ChunkMaskGenerator>()) {}

// Ocena pojedynczego chunka
float ChunkEvaluator::evaluateChunk(const Chunk& chunk, const BoardSnapshot& snapshot) const {
    // Suma feature'ów chunku, dodatkowe bonusy/maski
    float score = 0.0f;
    for (float f : chunk.features) score += f;


    return score;
}

// Ocena wielu chunków z maską/cechami
std::vector<float> ChunkEvaluator::evaluateChunks(const std::vector<Chunk>& chunks, const BoardSnapshot& snapshot) const {
    std::vector<float> results;
    results.reserve(chunks.size());
    for (const Chunk& chunk : chunks)
        results.push_back(evaluateChunk(chunk, snapshot));
    return results;
}

// Helper: ocena starych chunków przez konwersję na maskowane
std::vector<float> ChunkEvaluator::evaluateChunks(const std::vector<std::array<std::array<Piece, 2>, 2>>& arrays, const BoardSnapshot& snapshot) const {
    std::vector<Chunk> chunks = ArraysToChunks(arrays);
    return evaluateChunks(chunks, snapshot);
}

// Rozdzielanie bitboardu na chunkowe fragmenty (przykład)
std::array<uint64_t, NUM_CHUNKS> ChunkEvaluator::splitBitboardToChunks(uint64_t bitboard) const {
    std::array<uint64_t, NUM_CHUNKS> chunks{};
    int squaresPerChunk = 64 / NUM_CHUNKS;
    for (int i = 0; i < NUM_CHUNKS; ++i) {
        chunks[i] = (bitboard >> (i * squaresPerChunk)) & ((1ULL << squaresPerChunk) - 1);
    }
    return chunks;
}