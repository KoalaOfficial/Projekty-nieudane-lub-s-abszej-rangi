<<<<<<< HEAD
#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <vector>
#include <cstdint>
#include <array>
#include "../conAnHea/constants.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/build_chunks.h"

struct Move {
    int from;
    int to;
    PieceType promotionPiece = PieceType::EMPTY;
};

struct BoardSnapshot {
    uint64_t whiteBitboard = 0ULL;
    uint64_t blackBitboard = 0ULL;
    Color sideToMove = Color::WHITE;
    int enPassantSquare = -1;
    bool canCastleKingside[2] = {true, true};
    bool canCastleQueenside[2] = {true, true};
    uint64_t hash = 0ULL;

    // Osobne bitboardy dla każdego typu figury
    uint64_t whitePawnBitboard   = 0ULL;
    uint64_t whiteKnightBitboard = 0ULL;
    uint64_t whiteBishopBitboard = 0ULL;
    uint64_t whiteRookBitboard   = 0ULL;
    uint64_t whiteQueenBitboard  = 0ULL;
    uint64_t whiteKingBitboard   = 0ULL;

    uint64_t blackPawnBitboard   = 0ULL;
    uint64_t blackKnightBitboard = 0ULL;
    uint64_t blackBishopBitboard = 0ULL;
    uint64_t blackRookBitboard   = 0ULL;
    uint64_t blackQueenBitboard  = 0ULL;
    uint64_t blackKingBitboard   = 0ULL;
};

bool isMoveLegal(uint64_t fromMask, uint64_t toMask, const Piece& piece, uint64_t ownPieces, uint64_t opponentPieces);

std::vector<Move> GenerateAlpBetMoves(BoardSnapshot& board,
    std::vector<std::array<std::array<Piece, 2>, 2>>& chunks,
    Color currentColor);

inline std::vector<Move> GenerateAlpBetMoves_Masked(BoardSnapshot& board,
    std::vector<Chunk>& chunks,
    Color currentColor) {
    auto arrays = ChunksToArrays(chunks);
    return GenerateAlpBetMoves(board, arrays, currentColor);
}

// Poprawione deklaracje:
BoardSnapshot createBoardSnapshot(const std::pair<uint64_t, uint64_t>& bitboards);

void fillBoardSnapshot(
    BoardSnapshot& snapshot,
    const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks);

=======
#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <vector>
#include <cstdint>
#include <array>
#include "../conAnHea/constants.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/build_chunks.h"

struct Move {
    int from;
    int to;
    PieceType promotionPiece = PieceType::EMPTY;
};

struct BoardSnapshot {
    uint64_t whiteBitboard = 0ULL;
    uint64_t blackBitboard = 0ULL;
    Color sideToMove = Color::WHITE;
    int enPassantSquare = -1;
    bool canCastleKingside[2] = {true, true};
    bool canCastleQueenside[2] = {true, true};
    uint64_t hash = 0ULL;

    // Osobne bitboardy dla każdego typu figury
    uint64_t whitePawnBitboard   = 0ULL;
    uint64_t whiteKnightBitboard = 0ULL;
    uint64_t whiteBishopBitboard = 0ULL;
    uint64_t whiteRookBitboard   = 0ULL;
    uint64_t whiteQueenBitboard  = 0ULL;
    uint64_t whiteKingBitboard   = 0ULL;

    uint64_t blackPawnBitboard   = 0ULL;
    uint64_t blackKnightBitboard = 0ULL;
    uint64_t blackBishopBitboard = 0ULL;
    uint64_t blackRookBitboard   = 0ULL;
    uint64_t blackQueenBitboard  = 0ULL;
    uint64_t blackKingBitboard   = 0ULL;
};

bool isMoveLegal(uint64_t fromMask, uint64_t toMask, const Piece& piece, uint64_t ownPieces, uint64_t opponentPieces);

std::vector<Move> GenerateAlpBetMoves(BoardSnapshot& board,
    std::vector<std::array<std::array<Piece, 2>, 2>>& chunks,
    Color currentColor);

inline std::vector<Move> GenerateAlpBetMoves_Masked(BoardSnapshot& board,
    std::vector<Chunk>& chunks,
    Color currentColor) {
    auto arrays = ChunksToArrays(chunks);
    return GenerateAlpBetMoves(board, arrays, currentColor);
}

// Poprawione deklaracje:
BoardSnapshot createBoardSnapshot(const std::pair<uint64_t, uint64_t>& bitboards);

void fillBoardSnapshot(
    BoardSnapshot& snapshot,
    const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks);

>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
#endif // MOVE_GEN_H