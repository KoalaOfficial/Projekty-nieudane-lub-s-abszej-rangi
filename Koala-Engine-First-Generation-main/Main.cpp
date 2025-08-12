#include <sys/stat.h>
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <condition_variable>
#include <map>
#include <stdexcept>
#include <atomic>
#include <cmath>
#include "Listener.h"
#include "Parser.h"
#include "definitions.h" // Upewnij się, że masz odpowiednie pliki nagłówkowe
#include "events.h"

using namespace std;

std::mutex mtx;
bool checkFileExists(const string& filename);

class Position {
public:
    static const int PIECE_TYPE_NB = 6;
    static const int COLOR_NB = 2;
    static const int SQUARE_NB = 64;
    static const int White = 0;
    static const int Black = 1;
    static const int King = 5;
    static const int Queen = 4;
    static const int Rook = 3;
    static const int Bishop = 2;
    static const int Knight = 1;
    static const int Pawn = 0;
    static const uint64_t Empty = 0;

    uint64_t ZobristTable[SQUARE_NB][PIECE_TYPE_NB][COLOR_NB];
    uint64_t board[COLOR_NB];
    vector<int> moveHistory;
    int sideToMove;
    bool canCastle[COLOR_NB][2];
    int enPassantSquare;
    uint64_t hashKey;
    vector<Position> positionMemory;

    Position() {
        reset();
    }

    void reset() {
        board[White] = 0;
        board[Black] = 0;
        moveHistory.clear();
        sideToMove = White;
        canCastle[White][0] = canCastle[White][1] = true;
        canCastle[Black][0] = canCastle[Black][1] = true;
        enPassantSquare = -1;
        hashKey = 0;
        positionMemory.clear();
        initializeZobrist();
    }

    void makeMove(int move);
    void undoMove();
    void updateBoard(int fromSquare, int toSquare, int movedPiece, bool isUndo);
    void generatePieceMoves(int square, vector<int>& moves) const;
    bool isSquareAttacked(int square, int color) const;
    bool isMoveLegal(int move, int color) const;
    bool isEnPassantCapture(int fromSquare, int toSquare, int color) const;
    bool isInCheck(int color) const;
    bool isCheckmate(int color) const;
    bool isStalemate(int color) const;
    vector<int> generateMoves() const;
    vector<int> generateCandidateMoves() const;
    int mobilityScore(int color) const;
    int kingSafetyScore(int color) const;
    int evaluate() const;
    int evaluateGeneral() const;
    vector<int> generateCaptures() const;
    int pieceActivityScore(int color) const;
    int controlCenterScore() const;
    bool isSimilarTo(const Position& other) const;
    int QSearch(int alpha, int beta);

private:
    void initializeZobrist();
    int getPositionValue(int square, int color) const;
    int findKingSquare(int color) const;
    bool isRookAttacking(int square, int color) const;
    bool isBishopAttacking(int square, int color) const;
    bool isQueenAttacking(int square, int color) const;
    bool isKnightAttacking(int square, int color) const;
    bool isKingAttacking(int square, int color) const;
    void addPositionToMemory();
};

void Position::addPositionToMemory() {
    if (positionMemory.size() >= 10) {
        positionMemory.erase(positionMemory.begin());
    }
    positionMemory.push_back(*this);
}

bool Position::isSimilarTo(const Position& other) const {
    return hashKey == other.hashKey;
}

void Position::undoMove() {
    if (moveHistory.empty()) return;

    int move = moveHistory.back();
    moveHistory.pop_back();
    int fromSquare = (move >> 6) & 0x3F;
    int toSquare = move & 0x3F;
    int movedPiece = (move >> 12) & 0x0F;

    updateBoard(toSquare, fromSquare, movedPiece, true);

    if (movedPiece == Pawn) {
        if (toSquare == enPassantSquare) {
            int capturedSquare = fromSquare - 8 * (sideToMove == White ? 1 : -1);
            board[Black ^ sideToMove] |= (1ULL << capturedSquare);
}

    hashKey ^= ZobristTable[fromSquare][movedPiece][sideToMove];
    hashKey ^= ZobristTable[toSquare][movedPiece][sideToMove];
}

    hashKey ^= ZobristTable[fromSquare][movedPiece][sideToMove];
    hashKey ^= ZobristTable[toSquare][movedPiece][sideToMove];
}

bool Position::isRookAttacking(int square, int color) const {
    for (int dir : { 1, -1, 8, -8 }) {
        for (int step = 1; step < 8; ++step) {
            int targetSquare = square + dir * step;
            if (targetSquare < 0 || targetSquare >= SQUARE_NB) break;
            if ((board[color] >> targetSquare) & 1ULL) return true;
            if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break;
        }
    }
    return false;
}


bool Position::isBishopAttacking(int square, int color) const {
    for (int dir : { 7, 9, -7, -9 }) {
        for (int step = 1; step < 8; ++step) {
            int targetSquare = square + dir * step;
            if (targetSquare < 0 || targetSquare >= SQUARE_NB) break;
            if ((board[color] >> targetSquare) & 1ULL) return true;
            if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break;
        }
    }
    return false;
}

bool Position::isQueenAttacking(int square, int color) const {
    return isRookAttacking(square, color) || isBishopAttacking(square, color);
}

bool Position::isKnightAttacking(int square, int color) const {
    int knightMoves[8] = { 15, 17, -15, -17, 10, 6, -10, -6 };
    for (int move : knightMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < SQUARE_NB) {
            if ((board[color] >> targetSquare) & 1ULL) {
                return true;
            }
        }
    }
    return false;
}


bool Position::isKingAttacking(int square, int color) const {
    int kingMoves[8] = { 1, -1, 8, -8, 9, -9, 7, -7 };
    for (int move : kingMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < SQUARE_NB) {
            if ((board[color] >> targetSquare) & 1ULL) {
                return true;
            }
        }
    }
    return false;
}

bool Position::isMoveLegal(int move, int color) const {
    Position tempPos = *this;
    tempPos.makeMove(move);
    return !tempPos.isSquareAttacked(move & 0x3F, color);
}

bool Position::isEnPassantCapture(int fromSquare, int toSquare, int color) const {
    return (toSquare == enPassantSquare && abs(fromSquare - toSquare) == 1);
}

bool Position::isInCheck(int color) const {
    int kingSquare = findKingSquare(color);
    return isSquareAttacked(kingSquare, (color == White) ? Black : White);
}

bool Position::isCheckmate(int color) const {
    return isInCheck(color) && generateMoves().empty();
}

bool Position::isStalemate(int color) const {
    return !isInCheck(color) && generateMoves().empty();
}

int Position::findKingSquare(int color) const {
    for (int square = 0; square < SQUARE_NB; ++square) {
        if ((board[color] >> square) & 1ULL) {
            return square;
        }
      }
return -1; 

}

bool Position::isSquareAttacked(int square, int color) const {
    int direction = (color == White) ? -1 : 1;
    if ((board[Black] >> (square + direction * 8 - 1) & 1ULL) && color == White) return true;
    if ((board[Black] >> (square + direction * 8 + 1) & 1ULL) && color == White) return true;

    if (isRookAttacking(square, color) || isQueenAttacking(square, color)) return true;
    if (isBishopAttacking(square, color) || isQueenAttacking(square, color)) return true;
    if (isKnightAttacking(square, color)) return true;
    if (isKingAttacking(square, color)) return true;

    return false;
}

void Position::makeMove(int move) {
    moveHistory.push_back(move);
    int fromSquare = (move >> 6) & 0x3F;
    int toSquare = move & 0x3F;
    int movedPiece = (move >> 12) & 0x0F;

    updateBoard(fromSquare, toSquare, movedPiece, false);

    if (movedPiece == Pawn) {
        if (toSquare == enPassantSquare) {
            int capturedSquare = fromSquare + 8 * (sideToMove == White ? 1 : -1);
            board[Black ^ sideToMove] ^= (1ULL << capturedSquare);
            enPassantSquare = -1;
        }
if (toSquare == fromSquare + 16 or toSquare == fromSquare - 16) {
            enPassantSquare = (fromSquare + toSquare) / 2;
        } else {
            enPassantSquare = -1;
        }
if (toSquare < 8 or toSquare >= 56) {
            movedPiece = Queen;
}

    if (movedPiece == King) {
        if (toSquare == 6) {
            board[sideToMove] |= (1ULL << 5);
            board[sideToMove] ^= (1ULL << 7);
        } else if (toSquare == 2) {
            board[sideToMove] |= (1ULL << 3);
            board[sideToMove] ^= (1ULL << 0);
        }
        canCastle[sideToMove][0] = false;
        canCastle[sideToMove][1] = false;
    }

    hashKey ^= ZobristTable[fromSquare][movedPiece][sideToMove];
    hashKey ^= ZobristTable[toSquare][movedPiece][sideToMove];

    addPositionToMemory();
    sideToMove = Black ^ sideToMove;
}
if (toSquare == fromSquare + 16 or toSquare == fromSquare - 16) {
            enPassantSquare = (fromSquare + toSquare) / 2; 
        } else {
            enPassantSquare = -1; 
        }
if (toSquare < 8 or toSquare >= 56) {
            movedPiece = Queen; 
}

    if (movedPiece == King) {
        if (toSquare == 6) { 
            board[sideToMove] |= (1ULL << 5);
            board[sideToMove] ^= (1ULL << 7);
        } else if (toSquare == 2) { 
            board[sideToMove] |= (1ULL << 3);
            board[sideToMove] ^= (1ULL << 0);
        }
        canCastle[sideToMove][0] = false;
        canCastle[sideToMove][1] = false;
    }

    hashKey ^= ZobristTable[fromSquare][movedPiece][sideToMove];
    hashKey ^= ZobristTable[toSquare][movedPiece][sideToMove];

    addPositionToMemory();
    sideToMove = Black ^ sideToMove;
}

void Position::updateBoard(int fromSquare, int toSquare, int movedPiece, bool isUndo) {
    if (isUndo) {
        board[sideToMove] ^= (1ULL << toSquare);
        board[sideToMove] |= (1ULL << fromSquare);
    } else {
        board[sideToMove] ^= (1ULL << fromSquare);
        board[sideToMove] |= (1ULL << toSquare);
    }

    if (movedPiece == Pawn) {
        if (toSquare == enPassantSquare) {
            int capturedSquare = fromSquare - 8 * (sideToMove == White ? 1 : -1);
            board[Black ^ sideToMove] |= (1ULL << capturedSquare);
        }
if (toSquare == fromSquare + 16 or toSquare == fromSquare - 16) {
            enPassantSquare = (fromSquare + toSquare) / 2; 
        } else {
            enPassantSquare = -1; 
}

    hashKey ^= ZobristTable[fromSquare][movedPiece][sideToMove];
    hashKey ^= ZobristTable[toSquare][movedPiece][sideToMove];
}
}


void Position::generatePieceMoves(int square, vector<int>& moves) const {
    for (int i = 0; i < SQUARE_NB; ++i) {
        if ((board[sideToMove] >> i) & 1ULL) {
            if (i < 56) { int forwardMove = i + 8; if (!isSquareAttacked(forwardMove, sideToMove)) { moves.push_back((i << 6) | forwardMove); } }
                int forwardMove = i + 8;
                if (!isSquareAttacked(forwardMove, sideToMove)) {
                    moves.push_back((i << 6) | forwardMove);
                }
                if (i >= 48) { int doubleForwardMove = i + 16; if (!isSquareAttacked(doubleForwardMove, sideToMove)) { moves.push_back((i << 6) | doubleForwardMove); } }
                    int doubleForwardMove = i + 16;
                    if (!isSquareAttacked(doubleForwardMove, sideToMove)) {
                        moves.push_back((i << 6) | doubleForwardMove);
                }               
            }
            if ((board[sideToMove] >> i) & (1ULL << Knight)) { int knightMoves[] = { i + 17, i + 15, i - 17, i - 15, i + 10, i - 10, i + 6, i - 6 }; for (int move : knightMoves) { if (move >= 0 && move < SQUARE_NB && !isSquareAttacked(move, sideToMove)) { moves.push_back((i << 6) | move); } } }
                int knightMoves[] = { i + 17, i + 15, i - 17, i - 15, i + 10, i - 10, i + 6, i - 6 };
                for (int move : knightMoves) {
                    if (move >= 0 && move < SQUARE_NB && !isSquareAttacked(move, sideToMove)) {
                        moves.push_back((i << 6) | move);
                   }
            }
            if ((board[sideToMove] >> i) & (1ULL << Rook)) { for (int dir : {-1, 1, -8, 8}) { for (int step = 1; step < 8; ++step) { int targetSquare = i + dir * step; if (targetSquare < 0 || targetSquare >= SQUARE_NB || (dir == -1 && (i % 8 == 0)) || (dir == 1 && (i % 8 == 7))) break; if (isSquareAttacked(targetSquare, sideToMove)) { continue; } moves.push_back((i << 6) | targetSquare); if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break; } } }
                for (int dir : {-1, 1, -8, 8}) {
                    for (int step = 1; step < 8; ++step) {
                        int targetSquare = i + dir * step;
                        if (targetSquare < 0 || targetSquare >= SQUARE_NB || 
                            (dir == -1 && (i % 8 == 0)) || 
                            (dir == 1 && (i % 8 == 7))) break;
                        if (isSquareAttacked(targetSquare, sideToMove)) {
                            continue; 
                        }
                        moves.push_back((i << 6) | targetSquare);
                        if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break;
}
            }
            if ((board[sideToMove] >> i) & (1ULL << Bishop)) { for (int dir : {-9, -7, 7, 9}) { for (int step = 1; step < 8; ++step) { int targetSquare = i + dir * step; if (targetSquare < 0 || targetSquare >= SQUARE_NB || (dir == -9 && (i % 8 == 0)) || (dir == -7 && (i % 8 == 7)) || (dir == 7 && (i % 8 == 7)) || (dir == 9 && (i % 8 == 0))) break; if (isSquareAttacked(targetSquare, sideToMove)) { continue; } moves.push_back((i << 6) | targetSquare); if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break; } } }
                for (int dir : {-9, -7, 7, 9}) {
                    for (int step = 1; step < 8; ++step) {
                        int targetSquare = i + dir * step;
                        if (targetSquare < 0 || targetSquare >= SQUARE_NB || 
                            (dir == -9 && (i % 8 == 0)) || 
                            (dir == -7 && (i % 8 == 7)) || 
                            (dir == 7 && (i % 8 == 7)) || 
                            (dir == 9 && (i % 8 == 0))) break;
                        if (isSquareAttacked(targetSquare, sideToMove)) {
                            continue; 
                        }
                        moves.push_back((i << 6) | targetSquare);
                        if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break;
}
            }
            if ((board[sideToMove] >> i) & (1ULL << Queen)) { for (int dir : {-1, 1, -8, 8, -9, -7, 7, 9}) { for (int step = 1; step < 8; ++step) { int targetSquare = i + dir * step; if (targetSquare < 0 || targetSquare >= SQUARE_NB || (dir == -1 && (i % 8 == 0)) || (dir == 1 && (i % 8 == 7)) || (dir == -8 && (i < 8)) || (dir == 8 && (i >= 56))) break; if (isSquareAttacked(targetSquare, sideToMove)) { continue; } moves.push_back((i << 6) | targetSquare); if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break; } } }
                for (int dir : {-1, 1, -8, 8, -9, -7, 7, 9}) {
                    for (int step = 1; step < 8; ++step) {
                        int targetSquare = i + dir * step;
                        if (targetSquare < 0 || targetSquare >= SQUARE_NB || 
                            (dir == -1 && (i % 8 == 0)) || 
                            (dir == 1 && (i % 8 == 7)) || 
                            (dir == -8 && (i < 8)) || 
                            (dir == 8 && (i >= 56))) break;
                        if (isSquareAttacked(targetSquare, sideToMove)) {
                            continue; 
                        }
                        moves.push_back((i << 6) | targetSquare);
                        if (((board[White] | board[Black]) >> targetSquare) & 1ULL) break;
}
            }
            if (!isInCheck(sideToMove)) { if (!isSquareAttacked(4, Black) && !isSquareAttacked(5, Black) && !isSquareAttacked(6, Black) && (board[sideToMove] & (1ULL << 0)) && (board[sideToMove] & (1ULL << 4))) { moves.push_back((4 << 6) | 6); } if (!isSquareAttacked(2, Black) && !isSquareAttacked(1, Black) && !isSquareAttacked(0, Black) && (board[sideToMove] & (1ULL << 7)) && (board[sideToMove] & (1ULL << 4))) { moves.push_back((4 << 6) | 2); } }
                if (!isSquareAttacked(4, Black) && !isSquareAttacked(5, Black) && 
                    !isSquareAttacked(6, Black) && (board[sideToMove] & (1ULL << 0)) && 
                    (board[sideToMove] & (1ULL << 4))) {
                    moves.push_back((4 << 6) | 6);
                }
                if (!isSquareAttacked(2, Black) && !isSquareAttacked(1, Black) && 
                    !isSquareAttacked(0, Black) && (board[sideToMove] & (1ULL << 7)) && 
                    (board[sideToMove] & (1ULL << 4))) {
                    moves.push_back((4 << 6) | 2);
}
            if ((board[sideToMove] >> i) & (1ULL << King)) { int kingMoves[] = { i + 1, i - 1, i + 8, i - 8, i + 9, i - 9, i + 7, i - 7 }; for (int move : kingMoves) { if (move >= 0 && move < SQUARE_NB && !isSquareAttacked(move, sideToMove)) { moves.push_back((i << 6) | move); } } }
                int kingMoves[] = { i + 1, i - 1, i + 8, i - 8, i + 9, i - 9, i + 7, i - 7 };
                for (int move : kingMoves) {
                    if (move >= 0 && move < SQUARE_NB && !isSquareAttacked(move, sideToMove)) {
                        moves.push_back((i << 6) | move);
}
}
}

std::vector<int> Position::generateMoves() const {
    std::vector<int> moves;
    for (int i = 0; i < SQUARE_NB; ++i) {
        if ((board[sideToMove] >> i) & 1ULL) {
            generatePieceMoves(i, moves);
        }
    }
    return moves;
}


vector<int> Position::generateCandidateMoves() const {
    vector<int> candidateMoves;
    vector<int> allMoves = generateMoves();
    for (int i = 0; i < 5 && i < allMoves.size(); ++i) {
        candidateMoves.push_back(allMoves[i]);
    }
    return candidateMoves;
}

void Position::initializeZobrist() {
    HCRYPTPROV hProv;
    CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

    for (int square = 0; square < SQUARE_NB; ++square) {
        for (int piece = 0; piece < PIECE_TYPE_NB; ++piece) {
            for (int color = 0; color < COLOR_NB; ++color) {
                uint64_t randomValue;
                CryptGenRandom(hProv, sizeof(randomValue), reinterpret_cast<BYTE*>(&randomValue));
                ZobristTable[square][piece][color] = randomValue;
}
    }
         }
    CryptReleaseContext(hProv, 0);
}

int Position::mobilityScore(int color) const {
    int score = 0;
    vector<int> moves = generateMoves();
    score += moves.size() * 10; 
    return score;
}

int Position::kingSafetyScore(int color) const {
    int kingSquare = findKingSquare(color);
    int score = 0;

    if (isSquareAttacked(kingSquare, (color == White) ? Black : White)) {
        score -= 50; 
    }
    return score;
}

int Position::evaluate() const {
    int materialValue = evaluateGeneral();
    return materialValue;
}

int Position::evaluateGeneral() const {
    int materialValue = 0;

    for (int square = 0; square < SQUARE_NB; ++square) {
        for (int color = 0; color < COLOR_NB; ++color) {
            if ((board[color] >> square) & 1ULL) {
                materialValue += (color == White ? 1 : -1) * (PIECE_TYPE_NB - square);
                materialValue += getPositionValue(square, color);
}
    }

    materialValue += mobilityScore(White) - mobilityScore(Black);
    materialValue += kingSafetyScore(White) - kingSafetyScore(Black);
    materialValue += pieceActivityScore(White) - pieceActivityScore(Black);
    materialValue += controlCenterScore(); 

    return materialValue;
}

int Position::controlCenterScore() const {
    int score = 0;
    std::vector<int> centerSquares = {28, 36, 27, 35}; 
    for (int square : centerSquares) {
        if ((board[White] >> square) & 1ULL) {
            score += 10; 
        }
        if ((board[Black] >> square) & 1ULL) {
            score -= 10; 
}
}
    return score;

}

int Position::getPositionValue(int square, int color) const {
    int pieceValue = 0;

    const int pawnValue = 100;
    const int knightValue = 320;
    const int bishopValue = 330;
    const int rookValue = 500;
    const int queenValue = 900;
    const int kingValue = 20000;

    int pieceType = -1;
    if ((board[color] >> square) & 1ULL) pieceType = Pawn;
    else if ((board[Black] >> square) & 1ULL) pieceType = Knight;
    else if ((board[Black] >> square) & 1ULL) pieceType = Bishop;
    else if ((board[Black] >> square) & 1ULL) pieceType = Rook;
    else if ((board[Black] >> square) & 1ULL) pieceType = Queen;
    else if ((board[Black] >> square) & 1ULL) pieceType = King;

    switch (pieceType) {
        case Pawn:
            pieceValue += pawnValue;
            pieceValue += (color == White) ? (square / 8) * 10 : (7 - square / 8) * 10;
            break;
        case Knight:
            pieceValue += knightValue;
            pieceValue += (square == 27 || square == 28 || square == 35 || square == 36) ? 30 : 0;
            break;
        case Bishop:
            pieceValue += bishopValue;
            pieceValue += (square % 8 < 3 || square % 8 > 4) ? 15 : 0;
            break;
        case Rook:
            pieceValue += rookValue;
            pieceValue += (square % 8 == 0 || square % 8 == 7) ? 20 : 0;
            break;
        case Queen:
            pieceValue += queenValue;
            pieceValue += (square >= 27 && square <= 36) ? 20 : 0;
            break;
        case King:
            pieceValue += kingValue;
            break;
        default:
            break;
    }

    pieceValue *= (color == White) ? 1 : -1;

    return pieceValue;
}

class TranspositionTable {
public:
    struct Entry {
        int value;
        int depth;
        string nodeType;
        vector<int> bestMoves;
    };

    unordered_map<uint64_t, Entry> table;

    void store(uint64_t hashKey, int value, int depth, const string& nodeType, const vector<int>& bestMoves) {
        table[hashKey] = { value, depth, nodeType, bestMoves };
    }

    Entry* lookup(uint64_t hashKey) {
        auto it = table.find(hashKey);
        if (it != table.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void storeBestMove(uint64_t hashKey, const vector<int>& bestMoves) {
        if (table.find(hashKey) != table.end()) {
            table[hashKey].bestMoves = bestMoves;
}
};

class ChessEngine {
public:
    Position position;
    TranspositionTable transpositionTable;

    void makeMove(int move) {
        position.makeMove(move);
    }

    void undoMove() {
        position.undoMove();
    }

    int evaluate() const {
        return position.evaluate();
    }

    vector<int> generateMoves() const {
        return position.generateMoves();
    }
    
    vector<int> generateCandidateMoves() const {
        return position.generateCandidateMoves();
    }

    int searchBestMove(int depth) {
        return alphaBeta(depth, -10000, 10000);
    }

    int chooseMoveBasedOnPosition() {
        if (transpositionTable.lookup(position.hashKey)) {
            auto entry = transpositionTable.lookup(position.hashKey);
            if (!entry->bestMoves.empty()) {
                return entry->bestMoves[0]; 
}

        return searchBestMove(3); 
    }

    void compareWithDatabase() {
        auto entry = transpositionTable.lookup(position.hashKey);
        if (entry) {
            cout << "Position found in database with evaluation: " << entry->value << endl;
        } else {
            cout << "Position not found in database." << endl;
}

private:
    int alphaBeta(int depth, int alpha, int beta) {
        if (depth == 0) {
            return position.evaluate();
        }

        vector<int> moves = position.generateMoves();
        if (moves.empty()) {
            return position.isCheckmate(position.sideToMove) ? -10000 : 0;
        }

        auto entry = transpositionTable.lookup(position.hashKey);
        if (entry) {
            moves = entry->bestMoves; 
        }

        vector<int> bestMoves;
        for (int move : moves) {
            position.makeMove(move);
            int score = -alphaBeta(depth - 1, -beta, -alpha);
            position.undoMove();

            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
                bestMoves.clear();
                bestMoves.push_back(move);
}

        transpositionTable.storeBestMove(position.hashKey, bestMoves);
        return alpha;
    }

    int QSearch(int alpha, int beta); 
};

int Position::QSearch(int alpha, int beta) {
    int standPat = evaluate();
    if (standPat >= beta) return beta; 
    if (alpha < standPat) alpha = standPat; 

    vector<int> captures = generateCaptures(); 
    for (int move : captures) {
        makeMove(move);
        int score = -QSearch(-beta, -alpha);
        undoMove();
        if (score >= beta) return beta; 
        if (score > alpha) {
            alpha = score; 
}
    return alpha;
}

vector<int> Position::generateCaptures() const {
    vector<int> captures;
    for (int i = 0; i < SQUARE_NB; ++i) {
        if ((board[sideToMove] >> i) & 1ULL) {
            for (int target : generateMoves()) {
                if ((board[Black] >> (target & 0x3F)) & 1ULL) { 
                    captures.push_back(target);
}
}
    return captures;
}

int Position::pieceActivityScore(int color) const {
    int score = 0;
    for (int square = 0; square < SQUARE_NB; ++square) {
        if ((board[color] >> square) & 1ULL) {
            score += (color == White ? 1 : -1) * (7 - (square / 8));
}
    return score;
}

class TableBase {
public:
    struct PositionData {
        Position pos;
        int evaluation;
        vector<string> bestMoves;
    };

    TableBase() : exit(false), analysisActive(false) {
        saveThread = thread(&TableBase::saveLoop, this);
    }

    ~TableBase() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            exit = true;
        }
        cv.notify_one();
        saveThread.join();
    }

    void recordPosition(const Position& pos) {
        std::lock_guard<std::mutex> lock(mtx);
        PositionData posData;
        posData.pos = pos;
        posData.bestMoves = {};
        positions.push_back(posData);
        cv.notify_one();
    }

    void saveLoop() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !positions.empty() || exit; });

            if (exit) return;
            positions.clear();
}

    string posToString(const PositionData& posData) {
        string str = " | Eval: " + to_string(posData.evaluation);
        str += " | Best Moves: ";
        for (const auto& move : posData.bestMoves) {
            str += move + " ";
        }
        return str;
    }

    void setAnalysisActive(bool active) {
        std::lock_guard<std::mutex> lock(mtx);
        analysisActive = active;
    }

    bool isAnalysisActive() {
        std::lock_guard<std::mutex> lock(mtx);
        return analysisActive;
    }

private:
    vector<PositionData> positions;
    std::mutex mtx;  
    std::condition_variable cv;
    std::thread saveThread;
    std::atomic<bool> exit;
    std::atomic<bool> analysisActive;
};

class Worker {
public:
    ChessEngine* engine;
    int depth;
    int bestMove;

    Worker(ChessEngine* engine, int depth) : engine(engine), depth(depth), bestMove(0) {}

    void operator()() {
        bestMove = engine->searchBestMove(depth);
        engine->compareWithDatabase();
    }

    int getBestMove() const {
        return bestMove;
};

void threadedMoveSearch(ChessEngine& engine, int depth) {
    vector<thread> threads;
    vector<Worker> workers;

    for (int i = 0; i < 10; ++i) {
        workers.emplace_back(&engine, depth);
        threads.emplace_back(ref(workers.back()));
    }

    for (auto& t : threads) {
        t.join();
    }

    vector<int> bestMoves;
    for (const auto& worker : workers) {
        bestMoves.push_back(worker.getBestMove());
}

class Config {
public:
    void load(const string& filename);
    string getOption(const string& key) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : "";
    }

private:
    unordered_map<string, string> options; 
};

// Implementacja metody Config::load
void Config::load(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open config file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);
        string key, value;
        if (getline(ss, key, '=') && getline(ss, value)) {
            key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
            options[key] = value;
}
}
// Funkcja generująca ciąg Prouheta–Thue–Morse’a
std::vector<int> thueMorse(int n) {
    std::vector<int> sequence(1 << n); // 2^n elementów
    for (int i = 0; i < (1 << n); ++i) {
        sequence[i] = __builtin_popcount(i) % 2; // Liczba jedynek w binarnym zapisie
    }
    return sequence;
}


class HashFunction {
public:
    static unsigned long long computeHash(const std::string& binaryString) {
        unsigned long long hashValue = 0;
        const unsigned long long mod = static_cast<unsigned long long>(pow(2, binaryString.length()));
        
        for (size_t i = 0; i < binaryString.length(); ++i) {
            int bit = binaryString[i] - '0';
            hashValue = (2 * hashValue + bit) % mod;
        }
        
        return hashValue;
};

class MemoryManager {
public:
    template<typename T>
    static T* allocate(size_t count) {
        return static_cast<T*>(operator new[](sizeof(T) * count));
    }

    template<typename T>
    static void deallocate(T* ptr) {
        operator delete[](ptr);
};

class Crypto {
public:
    static std::string generateKey() {
        HCRYPTPROV hProv;
        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            BYTE key[16]; 
            if (CryptGenRandom(hProv, sizeof(key), key)) {
                CryptReleaseContext(hProv, 0);
                return std::string(reinterpret_cast<char*>(key), sizeof(key));
            }
            CryptReleaseContext(hProv, 0);
        }
        std::cerr << "Failed to generate key." << std::endl;
        return "";
    }

    static std::string hash(const std::string& data) {
        HCRYPTPROV hProv;
        HCRYPTHASH hHash;
        BYTE hash[32]; 
        DWORD hashLen = sizeof(hash);
        
        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT) &&
            CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash) &&
            CryptHashData(hHash, reinterpret_cast<const BYTE*>(data.c_str()), data.size(), 0) &&
            CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return std::string(reinterpret_cast<char*>(hash), hashLen);
        }
        std::cerr << "Failed to hash data." << std::endl;
        return "";
};

class SecureDataHandler {
public:
    static void processData(const std::string& input) {
        // Generowanie klucza
        std::string key = Crypto::generateKey();
        std::cout << "Generated Key: " << key << std::endl;

        // Hashowanie danych
        std::string hashedData = Crypto::hash(input);
        std::cout << "Hashed Data: " << hashedData << std::endl;

        // Generowanie liczby losowej
        uint32_t randomNumber = GenerateRandomNumber();
        std::cout << "Generated Random Number: " << randomNumber << std::endl;

    }

private:
    static uint32_t GenerateRandomNumber() {
        HCRYPTPROV hProv;
        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            uint32_t randomNumber;
            if (CryptGenRandom(hProv, sizeof(randomNumber), (BYTE*)&randomNumber)) {
                CryptReleaseContext(hProv, 0);
                return randomNumber;
            }
            CryptReleaseContext(hProv, 0);
        }
        std::cerr << "Failed to generate random number." << std::endl;
        return 0; 
};

int main() {
    Config config;
    TableBase tableBase;
    SecureDataHandler::processData("Przykładowe dane do hashowania");
	try {
        config.load("Config.ini");

        int depth = stoi(config.getOption("depth"));
        int hash_size = stoi(config.getOption("hash_size"));
        int threads = stoi(config.getOption("threads"));
        int move_time = stoi(config.getOption("move_time"));
        int search_depth = stoi(config.getOption("search_depth"));
       int n = stoi(config.getOption("n"));

        ChessEngine engine;
		
// ten fragmentr UCI ma swoje źródło  z repozytorium andersfylling/uci. 
        // UCI Listener Setup
        uci::Listener listener;
        
      listener.addListener(uci::event::UCI, [&](std::map<std::string, std::string> args) {
    std::cout << "id name " << config.getOption("name") << std::endl
              << "id author " << config.getOption("author") << std::endl
              << "uciok" << std::endl;
});

      listener.addListener(uci::event::ISREADY, [](std::map<std::string, std::string> args) {
    std::cout << "readyok" << std::endl;
});

       listener.addListener(uci::event::GO, [&](std::map<std::string, std::string> args) {
    int depth = 5; // default depth
int move_time = -1;
    int nodes = -1;

    if (args.find("depth") != args.end()) {
        depth = std::stoi(args["depth"]);
    }
if (args.find("move_time") != args.end()) {
move_time = std::stoi(args["move_time"]);
    }
    if (args.find("nodes") != args.end()) {
        nodes = std::stoi(args["nodes"]);
    }

    int bestMove = engine.searchBestMove(depth);
    engine.makeMove(bestMove);
    std::cout << "info string Best move chosen: " << bestMove << std::endl;
    std::cout << "bestmove " << bestMove << std::endl;
});

       listener.addListener(uci::event::QUIT, [&](std::map<std::string, std::string> args) {
    listener.stopListening();
});

        listener.addListener(uci::event::SETOPTION, [](uci::arguments_t args) {
            // Handle setting options here, if needed
            cout << "Option set" << endl;
        });

        // Start listening for UCI commands
        listener.setupListener();
// koniec fragmentu z UCI
		
    return 0;
}
