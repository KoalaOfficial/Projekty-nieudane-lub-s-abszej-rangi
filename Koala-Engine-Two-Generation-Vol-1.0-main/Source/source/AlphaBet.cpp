#include "../conAnHea/AlphaBet.h"
#include <limits>
#include <algorithm>

// Helpers for special move types
bool isPromotionMove(const BoardSnapshot& board, const Move& move) {
    int fromRank = move.from / 8;
    int toRank = move.to / 8;
    return (board.sideToMove == Color::WHITE && fromRank == 6 && toRank == 7) ||
           (board.sideToMove == Color::BLACK && fromRank == 1 && toRank == 0);
}

bool isEnPassantMove(const BoardSnapshot& board, const Move& move) {
    return board.enPassantSquare >= 0 && move.to == board.enPassantSquare;
}

bool isCastlingMove(const BoardSnapshot& board, const Move& move) {
    if (board.sideToMove == Color::WHITE && move.from == 4 && (move.to == 6 || move.to == 2)) return true;
    if (board.sideToMove == Color::BLACK && move.from == 60 && (move.to == 62 || move.to == 58)) return true;
    return false;
}

// Applies a move to a board snapshot (modifies snapshot in place)
void performMove(BoardSnapshot& board, const Move& move) {
    uint64_t fromMask = 1ULL << move.from;
    uint64_t toMask = 1ULL << move.to;
    bool isWhite = (board.sideToMove == Color::WHITE);

    // Promotion
    if (isPromotionMove(board, move)) {
        if (isWhite) {
            board.whiteBitboard &= ~fromMask;
            board.whiteBitboard |= toMask;
            board.blackBitboard &= ~toMask;
        } else {
            board.blackBitboard &= ~fromMask;
            board.blackBitboard |= toMask;
            board.whiteBitboard &= ~toMask;
        }
        board.enPassantSquare = -1;
        board.sideToMove = isWhite ? Color::BLACK : Color::WHITE;
        return;
    }

    // En passant
    if (isEnPassantMove(board, move)) {
        if (isWhite) {
            board.whiteBitboard &= ~fromMask;
            board.whiteBitboard |= toMask;
            board.blackBitboard &= ~(1ULL << (move.to - 8));
        } else {
            board.blackBitboard &= ~fromMask;
            board.blackBitboard |= toMask;
            board.whiteBitboard &= ~(1ULL << (move.to + 8));
        }
        board.enPassantSquare = -1;
        board.sideToMove = isWhite ? Color::BLACK : Color::WHITE;
        return;
    }

    // Castling
    if (isCastlingMove(board, move)) {
        if (isWhite) {
            board.whiteBitboard &= ~fromMask;
            board.whiteBitboard |= toMask;
            if (move.to == 6) { // kingside
                board.whiteBitboard &= ~(1ULL << 7);
                board.whiteBitboard |= (1ULL << 5);
            } else if (move.to == 2) { // queenside
                board.whiteBitboard &= ~(1ULL << 0);
                board.whiteBitboard |= (1ULL << 3);
            }
            board.canCastleKingside[static_cast<int>(Color::WHITE)] = false;
            board.canCastleQueenside[static_cast<int>(Color::WHITE)] = false;
        } else {
            board.blackBitboard &= ~fromMask;
            board.blackBitboard |= toMask;
            if (move.to == 62) { // kingside
                board.blackBitboard &= ~(1ULL << 63);
                board.blackBitboard |= (1ULL << 61);
            } else if (move.to == 58) { // queenside
                board.blackBitboard &= ~(1ULL << 56);
                board.blackBitboard |= (1ULL << 59);
            }
            board.canCastleKingside[static_cast<int>(Color::BLACK)] = false;
            board.canCastleQueenside[static_cast<int>(Color::BLACK)] = false;
        }
        board.enPassantSquare = -1;
        board.sideToMove = isWhite ? Color::BLACK : Color::WHITE;
        return;
    }

    // Normal move (including capture)
    if (isWhite) {
        board.whiteBitboard &= ~fromMask;
        board.whiteBitboard |= toMask;
        board.blackBitboard &= ~toMask;
    } else {
        board.blackBitboard &= ~fromMask;
        board.blackBitboard |= toMask;
        board.whiteBitboard &= ~toMask;
    }

    int fromRank = move.from / 8, toRank = move.to / 8;
    if ((isWhite && fromRank == 1 && toRank == 3) || (!isWhite && fromRank == 6 && toRank == 4)) {
        board.enPassantSquare = isWhite ? (move.from + 8) : (move.from - 8);
    } else {
        board.enPassantSquare = -1;
    }

    if (isWhite) {
        if (move.from == 4) {
            board.canCastleKingside[static_cast<int>(Color::WHITE)] = false;
            board.canCastleQueenside[static_cast<int>(Color::WHITE)] = false;
        } else if (move.from == 7) {
            board.canCastleKingside[static_cast<int>(Color::WHITE)] = false;
        } else if (move.from == 0) {
            board.canCastleQueenside[static_cast<int>(Color::WHITE)] = false;
        }
    } else {
        if (move.from == 60) {
            board.canCastleKingside[static_cast<int>(Color::BLACK)] = false;
            board.canCastleQueenside[static_cast<int>(Color::BLACK)] = false;
        } else if (move.from == 63) {
            board.canCastleKingside[static_cast<int>(Color::BLACK)] = false;
        } else if (move.from == 56) {
            board.canCastleQueenside[static_cast<int>(Color::BLACK)] = false;
        }
    }
    board.sideToMove = isWhite ? Color::BLACK : Color::WHITE;
}

float vectorToFloat(const std::vector<float>& v) {
    if (v.empty()) return 0.0f;
    float sum = 0.0f;
    for (float x : v) sum += x;
    return sum / v.size();
}

SearchResult alphaBeta(
    BoardSnapshot snapshot,
    const std::vector<Chunk>& chunksEval,
    int depth,
    float alpha,
    float beta,
    bool maximizingPlayer,
    Color color,
    const ChunkEvaluator& chunkEvaluator,
    const ChunkFeatureExtractor& featureExtractor
) {
    SearchResult result;
    result.root = std::make_unique<TreeNode>(snapshot);

    std::vector<MoveEvaluation> evaluatedMoves;
    Move bestMove;
    float bestScore = maximizingPlayer ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();

    auto chunkArrays = ChunksToArrays(chunksEval);
    auto moves = GenerateAlpBetMoves(snapshot, chunkArrays, color);

    if (moves.empty() || depth == 0) {
        auto evals = chunkEvaluator.evaluateChunks(chunkArrays, snapshot);
        result.score = vectorToFloat(evals);
        result.root->score = result.score;
        result.foundMove = false;
        return result;
    }

    for (const Move& move : moves) {
        BoardSnapshot newSnapshot = snapshot;
        performMove(newSnapshot, move);

        auto newChunksEval = buildChunks(newSnapshot, featureExtractor);

        SearchResult childResult = alphaBeta(
            newSnapshot,
            newChunksEval,
            depth - 1,
            alpha,
            beta,
            !maximizingPlayer,
            (color == Color::WHITE ? Color::BLACK : Color::WHITE),
            chunkEvaluator,
            featureExtractor
        );

        result.root->children.push_back(std::move(childResult.root));
        float eval = childResult.score;
        evaluatedMoves.push_back({move, eval});

        if (maximizingPlayer) {
            if (eval > bestScore) {
                bestScore = eval;
                bestMove = move;
            }
            alpha = std::max(alpha, eval);
        } else {
            if (eval < bestScore) {
                bestScore = eval;
                bestMove = move;
            }
            beta = std::min(beta, eval);
        }
        if (beta <= alpha) break;
    }

    auto comparator = [maximizingPlayer](const MoveEvaluation& a, const MoveEvaluation& b) {
        return maximizingPlayer ? a.score > b.score : a.score < b.score;
    };
    std::sort(evaluatedMoves.begin(), evaluatedMoves.end(), comparator);

    result.score = bestScore;
    result.bestMove = bestMove;
    result.foundMove = !moves.empty();
    result.evaluatedMoves = evaluatedMoves;
    result.root->score = bestScore;
    return result;
}