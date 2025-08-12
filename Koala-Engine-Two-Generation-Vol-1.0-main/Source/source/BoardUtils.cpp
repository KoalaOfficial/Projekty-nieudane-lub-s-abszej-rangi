<<<<<<< HEAD
#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include "../conAnHea/constants.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/GameContextBoardH.h"
BoardUtils boardUtils; // <- to jest definicja, tylko jedna w całym projekcie!

using namespace std;



class BoardUtilss {
public:
    static std::pair<uint64_t, uint64_t> generateAllBitboards(
        const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
        uint64_t whiteBitboard = 0ULL;
        uint64_t blackBitboard = 0ULL;

        for (int chunkIndex = 0; chunkIndex < NUM_CHUNKS; ++chunkIndex) {
            const auto& chunk = chunks[chunkIndex];

            for (int row = 0; row < CHUNK_SIZE; ++row) {
                for (int col = 0; col < CHUNK_SIZE; ++col) {
                    int globalIndex = chunkIndex * CHUNK_SIZE * CHUNK_SIZE + row * CHUNK_SIZE + col;
                    const Piece& piece = chunk[row][col];

                    if (piece.type != PieceType::EMPTY) {
                        if (piece.color == Color::WHITE) {
                            whiteBitboard |= (1ULL << globalIndex);
                        } else if (piece.color == Color::BLACK) {
                            blackBitboard |= (1ULL << globalIndex);
                        }
                    }
                }
            }
        }
        return {whiteBitboard, blackBitboard};
    }
=======
#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include "../conAnHea/constants.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/GameContextBoardH.h"

using namespace std;

class BoardUtilss {
public:
    static std::pair<uint64_t, uint64_t> generateAllBitboards(
        const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks) {
        uint64_t whiteBitboard = 0ULL;
        uint64_t blackBitboard = 0ULL;

        for (int chunkIndex = 0; chunkIndex < NUM_CHUNKS; ++chunkIndex) {
            const auto& chunk = chunks[chunkIndex];

            for (int row = 0; row < CHUNK_SIZE; ++row) {
                for (int col = 0; col < CHUNK_SIZE; ++col) {
                    int globalIndex = chunkIndex * CHUNK_SIZE * CHUNK_SIZE + row * CHUNK_SIZE + col;
                    const Piece& piece = chunk[row][col];

                    if (piece.type != PieceType::EMPTY) {
                        if (piece.color == Color::WHITE) {
                            whiteBitboard |= (1ULL << globalIndex);
                        } else if (piece.color == Color::BLACK) {
                            blackBitboard |= (1ULL << globalIndex);
                        }
                    }
                }
            }
        }
        return {whiteBitboard, blackBitboard};
    }
>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
};