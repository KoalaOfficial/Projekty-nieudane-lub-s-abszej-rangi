<<<<<<< HEAD
#include <vector>
#include <cstdint>
#include <array>
#include "constants.h"
#include "ChunkFeatureExtractor.h"
#include "MoveGen.h"
#include "build_chunks.h"
#include "StrategicGrids.h"
#include "../conAnHea/build_chunks.h"



std::vector<std::array<std::array<Piece,2>,2>> ChunksToArrays(const std::vector<Chunk>& chunks) {
    std::vector<std::array<std::array<Piece,2>,2>> out;
    out.reserve(chunks.size());
    for (const auto& chunk : chunks)
        out.push_back(chunk.toArray());
    return out;
}

// Funkcja zamieniająca wektor tablic na wektor Chunków
std::vector<Chunk> ArraysToChunks(const std::vector<std::array<std::array<Piece, 2>, 2>>& arrays) {
    std::vector<Chunk> out;
    out.reserve(arrays.size());
    for (const auto& arr : arrays)
        out.push_back(Chunk::fromArray(arr));
    return out;
}

// Prosta, pomocnicza funkcja: konwertuje tablicę 2x2 Piece na Chunk z domyślnymi polami
Chunk Chunk::fromArray(const std::array<std::array<Piece, 2>, 2>& arr) {
    Chunk chunk;
    chunk.data = arr;
    chunk.mask = 0; // Jeśli nie masz logiki do maski, zostaw 0.
    chunk.id = 0;   // Jeśli nie masz logiki do id, zostaw 0.
    chunk.features.clear(); // Puste cechy, jeśli nie generujesz ich tutaj
    return chunk;
}

Piece getPieceAtSquare(int sq, const BoardSnapshot& snapshot) {
    // Białe figury
    if ((snapshot.whitePawnBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::PAWN);
    if ((snapshot.whiteKnightBitboard >> sq) & 1ULL) return Piece(Color::WHITE, PieceType::KNIGHT);
    if ((snapshot.whiteBishopBitboard >> sq) & 1ULL) return Piece(Color::WHITE, PieceType::BISHOP);
    if ((snapshot.whiteRookBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::ROOK);
    if ((snapshot.whiteQueenBitboard >> sq) & 1ULL)  return Piece(Color::WHITE, PieceType::QUEEN);
    if ((snapshot.whiteKingBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::KING);

    // Czarne figury
    if ((snapshot.blackPawnBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::PAWN);
    if ((snapshot.blackKnightBitboard >> sq) & 1ULL) return Piece(Color::BLACK, PieceType::KNIGHT);
    if ((snapshot.blackBishopBitboard >> sq) & 1ULL) return Piece(Color::BLACK, PieceType::BISHOP);
    if ((snapshot.blackRookBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::ROOK);
    if ((snapshot.blackQueenBitboard >> sq) & 1ULL)  return Piece(Color::BLACK, PieceType::QUEEN);
    if ((snapshot.blackKingBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::KING);

    return EMPTY_PIECE;
}

std::vector<Chunk> buildChunks(const BoardSnapshot& snapshot, const ChunkFeatureExtractor& extractor) {
    std::vector<Chunk> chunks;
    chunks.reserve(NUM_CHUNKS);

    constexpr int CHUNKS_PER_ROW = BOARD_SIZE / CHUNK_SIZE;

    for (int idx = 0; idx < NUM_CHUNKS; ++idx) {
        Chunk chunk;
        chunk.id = idx;

        // Wyznacz maskę dla chunku
        uint64_t mask = getChunkMask(idx);

        // Wyciągnij bitboardy ograniczone do chunku
        uint64_t whiteBB = snapshot.whiteBitboard & mask;
        uint64_t blackBB = snapshot.blackBitboard & mask;

        // Uzupełnij chunk.data prawdziwymi figurami
        int chunkRow = idx / CHUNKS_PER_ROW;
        int chunkCol = idx % CHUNKS_PER_ROW;
        for (int r = 0; r < CHUNK_SIZE; ++r) {
            for (int c = 0; c < CHUNK_SIZE; ++c) {
                int globalRow = chunkRow * CHUNK_SIZE + r;
                int globalCol = chunkCol * CHUNK_SIZE + c;
                int sq = globalRow * BOARD_SIZE + globalCol;
                chunk.data[r][c] = getPieceAtSquare(sq, snapshot);
            }
        }

        chunk.mask = mask;
        chunk.features = toVector(extractor.extractFeatures(idx, whiteBB, blackBB));
        chunks.push_back(chunk);
    }
    return chunks;
=======
#include <vector>
#include <cstdint>
#include <array>
#include "constants.h"
#include "ChunkFeatureExtractor.h"
#include "MoveGen.h"
#include "build_chunks.h"
#include "StrategicGrids.h"
#include "../conAnHea/build_chunks.h"

Piece getPieceAtSquare(int sq, const BoardSnapshot& snapshot) {
    // Białe figury
    if ((snapshot.whitePawnBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::PAWN);
    if ((snapshot.whiteKnightBitboard >> sq) & 1ULL) return Piece(Color::WHITE, PieceType::KNIGHT);
    if ((snapshot.whiteBishopBitboard >> sq) & 1ULL) return Piece(Color::WHITE, PieceType::BISHOP);
    if ((snapshot.whiteRookBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::ROOK);
    if ((snapshot.whiteQueenBitboard >> sq) & 1ULL)  return Piece(Color::WHITE, PieceType::QUEEN);
    if ((snapshot.whiteKingBitboard >> sq) & 1ULL)   return Piece(Color::WHITE, PieceType::KING);

    // Czarne figury
    if ((snapshot.blackPawnBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::PAWN);
    if ((snapshot.blackKnightBitboard >> sq) & 1ULL) return Piece(Color::BLACK, PieceType::KNIGHT);
    if ((snapshot.blackBishopBitboard >> sq) & 1ULL) return Piece(Color::BLACK, PieceType::BISHOP);
    if ((snapshot.blackRookBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::ROOK);
    if ((snapshot.blackQueenBitboard >> sq) & 1ULL)  return Piece(Color::BLACK, PieceType::QUEEN);
    if ((snapshot.blackKingBitboard >> sq) & 1ULL)   return Piece(Color::BLACK, PieceType::KING);

    return EMPTY_PIECE;
}

std::vector<Chunk> buildChunks(const BoardSnapshot& snapshot, const ChunkFeatureExtractor& extractor) {
    std::vector<Chunk> chunks;
    chunks.reserve(NUM_CHUNKS);

    constexpr int CHUNKS_PER_ROW = BOARD_SIZE / CHUNK_SIZE;

    for (int idx = 0; idx < NUM_CHUNKS; ++idx) {
        Chunk chunk;
        chunk.id = idx;

        // Wyznacz maskę dla chunku
        uint64_t mask = getChunkMask(idx);

        // Wyciągnij bitboardy ograniczone do chunku
        uint64_t whiteBB = snapshot.whiteBitboard & mask;
        uint64_t blackBB = snapshot.blackBitboard & mask;

        // Uzupełnij chunk.data prawdziwymi figurami
        int chunkRow = idx / CHUNKS_PER_ROW;
        int chunkCol = idx % CHUNKS_PER_ROW;
        for (int r = 0; r < CHUNK_SIZE; ++r) {
            for (int c = 0; c < CHUNK_SIZE; ++c) {
                int globalRow = chunkRow * CHUNK_SIZE + r;
                int globalCol = chunkCol * CHUNK_SIZE + c;
                int sq = globalRow * BOARD_SIZE + globalCol;
                chunk.data[r][c] = getPieceAtSquare(sq, snapshot);
            }
        }

        chunk.mask = mask;
        chunk.features = toVector(extractor.extractFeatures(idx, whiteBB, blackBB));
        chunks.push_back(chunk);
    }
    return chunks;
>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
}