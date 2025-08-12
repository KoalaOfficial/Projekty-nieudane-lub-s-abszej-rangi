#include "../conAnHea/MoveGen.h"
#include "../conAnHea/GameContextBoardH.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/Attacks.h"
#include "../conAnHea/SpecialMoves.h"

extern GameContext gameContext;
extern BoardUtils boardUtils;

BoardSnapshot snapshot;

void fillBoardSnapshot(
    BoardSnapshot& snapshot,
    const std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks)
{
    // Zerowanie wszystkich bitboardów
    snapshot.whiteBitboard = snapshot.blackBitboard = 0ULL;
    snapshot.whitePawnBitboard = snapshot.whiteKnightBitboard = snapshot.whiteBishopBitboard = 0ULL;
    snapshot.whiteRookBitboard = snapshot.whiteQueenBitboard = snapshot.whiteKingBitboard = 0ULL;
    snapshot.blackPawnBitboard = snapshot.blackKnightBitboard = snapshot.blackBishopBitboard = 0ULL;
    snapshot.blackRookBitboard = snapshot.blackQueenBitboard = snapshot.blackKingBitboard = 0ULL;

    for (int chunkIdx = 0; chunkIdx < NUM_CHUNKS; ++chunkIdx) {
        for (int r = 0; r < CHUNK_SIZE; ++r) {
            for (int c = 0; c < CHUNK_SIZE; ++c) {
                int globalRow = (chunkIdx / (BOARD_SIZE / CHUNK_SIZE)) * CHUNK_SIZE + r;
                int globalCol = (chunkIdx % (BOARD_SIZE / CHUNK_SIZE)) * CHUNK_SIZE + c;
                int sq = globalRow * BOARD_SIZE + globalCol;
                const Piece& piece = chunks[chunkIdx][r][c];
                if (piece.type == PieceType::EMPTY) continue;

                if (piece.color == Color::WHITE) {
                    snapshot.whiteBitboard |= (1ULL << sq);
                    switch (piece.type) {
                        case PieceType::PAWN:   snapshot.whitePawnBitboard   |= (1ULL << sq); break;
                        case PieceType::KNIGHT: snapshot.whiteKnightBitboard |= (1ULL << sq); break;
                        case PieceType::BISHOP: snapshot.whiteBishopBitboard |= (1ULL << sq); break;
                        case PieceType::ROOK:   snapshot.whiteRookBitboard   |= (1ULL << sq); break;
                        case PieceType::QUEEN:  snapshot.whiteQueenBitboard  |= (1ULL << sq); break;
                        case PieceType::KING:   snapshot.whiteKingBitboard   |= (1ULL << sq); break;
                        default: break;
                    }
                } else if (piece.color == Color::BLACK) {
                    snapshot.blackBitboard |= (1ULL << sq);
                    switch (piece.type) {
                        case PieceType::PAWN:   snapshot.blackPawnBitboard   |= (1ULL << sq); break;
                        case PieceType::KNIGHT: snapshot.blackKnightBitboard |= (1ULL << sq); break;
                        case PieceType::BISHOP: snapshot.blackBishopBitboard |= (1ULL << sq); break;
                        case PieceType::ROOK:   snapshot.blackRookBitboard   |= (1ULL << sq); break;
                        case PieceType::QUEEN:  snapshot.blackQueenBitboard  |= (1ULL << sq); break;
                        case PieceType::KING:   snapshot.blackKingBitboard   |= (1ULL << sq); break;
                        default: break;
                    }
                }
            }
        }
    }
}


BoardSnapshot createBoardSnapshot(const std::pair<uint64_t, uint64_t>& bitboards) {
    BoardSnapshot snapshot;
    snapshot.whiteBitboard = bitboards.first;
    snapshot.blackBitboard = bitboards.second;
    snapshot.hash = boardUtils.getCurrentHash();

    
    // Przechwytujemy stronę do ruchu
    snapshot.sideToMove = gameContext.currentColor; // do zmiany-funkja w menu

    // Pole en passant (-1 jeśli brak)
    snapshot.enPassantSquare = gameContext.epSquare;

    // Prawa roszady (przyjmujemy, że masz takie flagi w GameContext)
    snapshot.canCastleKingside[0] = snapshot.canCastleKingside[static_cast<int>(Color::WHITE)];
    snapshot.canCastleKingside[1] = snapshot.canCastleKingside[static_cast<int>(Color::BLACK)];
    snapshot.canCastleQueenside[0] = snapshot.canCastleQueenside[static_cast<int>(Color::WHITE)];
    snapshot.canCastleQueenside[1] = snapshot.canCastleQueenside[static_cast<int>(Color::BLACK)];

    // Promocje: snapshot nie musi ich pamiętać – MoveGen wygeneruje ruch promocyjny
    return snapshot;
}


// Pomocnicza funkcja do pobierania figury ze standardowego chunks
const Piece& getPieceFromChunks(const std::vector<std::array<std::array<Piece, 2>, 2>>& chunks, int sq) {
    int chunkIdx = gameContext.fieldProperties[sq].chunkIndex;
    int rowInChunk = gameContext.fieldProperties[sq].rowInChunk;
    int colInChunk = gameContext.fieldProperties[sq].colInChunk;
    return chunks[chunkIdx][rowInChunk][colInChunk];
}

std::vector<Move> GenerateAlpBetMoves(BoardSnapshot& board,
    std::vector<std::array<std::array<Piece, 2>, 2>>& chunks,
    Color currentColor) {
    std::vector<Move> moves;
    uint64_t own = (currentColor == Color::WHITE) ? board.whiteBitboard : board.blackBitboard;
    uint64_t opp = (currentColor == Color::WHITE) ? board.blackBitboard : board.whiteBitboard;
    uint64_t occupancy = own | opp;

    Square epSquare = gameContext.epSquare; // -1 jeśli brak
    bool canCastleKingside = board.canCastleKingside[static_cast<int>(currentColor)];
    bool canCastleQueenside = board.canCastleQueenside[static_cast<int>(currentColor)];

    for (int from = 0; from < 64; ++from) {
        if (!(own & (1ULL << from))) continue;
        const Piece& piece = getPieceFromChunks(chunks, from);

        std::vector<uint64_t> atts = generatePieceAttacks(from, piece, occupancy, own, opp);
        for (uint64_t att : atts) {
            for (int to = 0; to < 64; ++to) {
                if (att & (1ULL << to)) {
                    moves.push_back({from, to});
                }
            }
        }

        if (piece.type == PieceType::PAWN) {
            Bitboard promoTargets = generatePromotionAttacks(currentColor, from, ~occupancy);
            for (int to = 0; to < 64; ++to) {
                if (promoTargets & (1ULL << to)) {
                    moves.push_back({from, to, PieceType::QUEEN}); // domyślnie hetman
                }
            }
        }

        if (piece.type == PieceType::PAWN && epSquare >= 0 && epSquare < 64) {
            Bitboard epAttacks = generateEnPassantAttacks(currentColor, from, epSquare);
            if (epAttacks) {
                moves.push_back({from, epSquare});
            }
        }

        if (piece.type == PieceType::KING) {
            Bitboard castleMoves = generateCastlingAttacks(currentColor, occupancy, canCastleKingside, canCastleQueenside);
            for (int to = 0; to < 64; ++to) {
                if (castleMoves & (1ULL << to)) {
                    moves.push_back({from, to});
                }
            }
        }
    }
    return moves;
}