<<<<<<< HEAD
#ifndef BUILD_CHUNKS_H
#define BUILD_CHUNKS_H

#include <vector>
#include <array>
#include "constants.h"

struct BoardSnapshot;
class ChunkFeatureExtractor;

struct Chunk {
    std::array<std::array<Piece, 2>, 2> data;
    uint64_t mask;
    int id = 0;
    std::vector<float> features;
    std::array<std::array<Piece, 2>, 2> toArray() const { return data; }
    static Chunk fromArray(const std::array<std::array<Piece, 2>, 2>& arr);
};

std::vector<std::array<std::array<Piece,2>,2>> ChunksToArrays(const std::vector<Chunk>& chunks);
std::vector<Chunk> ArraysToChunks(const std::vector<std::array<std::array<Piece,2>,2>>& arrays);
std::vector<Chunk> buildChunks(const BoardSnapshot& snapshot, const ChunkFeatureExtractor& extractor);

=======
#ifndef BUILD_CHUNKS_H
#define BUILD_CHUNKS_H

#include <vector>
#include <array>
#include "constants.h"

struct BoardSnapshot;
class ChunkFeatureExtractor;

struct Chunk {
    std::array<std::array<Piece, 2>, 2> data;
    uint64_t mask;
    int id = 0;
    std::vector<float> features;
    std::array<std::array<Piece, 2>, 2> toArray() const { return data; }
    static Chunk fromArray(const std::array<std::array<Piece, 2>, 2>& arr);
};

std::vector<std::array<std::array<Piece,2>,2>> ChunksToArrays(const std::vector<Chunk>& chunks);
std::vector<Chunk> ArraysToChunks(const std::vector<std::array<std::array<Piece,2>,2>>& arrays);
std::vector<Chunk> buildChunks(const BoardSnapshot& snapshot, const ChunkFeatureExtractor& extractor);

>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
#endif // BUILD_CHUNKS_H