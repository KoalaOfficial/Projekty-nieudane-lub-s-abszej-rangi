#ifndef ATTACKS_H
#define ATTACKS_H

#include <array>
#include <cstdint>
#include "constants.h"
#include <vector>

extern std::array<uint64_t, 64> knightAttacks;
extern std::array<uint64_t, 64> kingAttacks;

// Funkcje
void initKnightAttacks();
void initKingAttacks();
uint64_t generateBishopAttacksDB(int sq, uint64_t occupancy);
uint64_t generateRookAttacksDB(int sq, uint64_t occupancy);
uint64_t generateQueenAttacks(int sq, uint64_t occupancy);
uint64_t generatePawnAttacks(int sq, const Piece& piece); // Generuje tylko ataki (bicia) pionków
uint64_t generateKnightAttacks(int sq);
uint64_t generateKingAttacks(int sq);
std::vector<uint64_t> generatePieceAttacks(int fromIndex, const Piece& piece, uint64_t occupancy, uint64_t ownPieces, uint64_t opponentPieces);


// Deklaracja funkcji do skanowania bitów (przeniesiona z Attacks.cpp, jeśli ma być używana globalnie)
int bitScanForward(uint64_t bb);



Bitboard generatePromotionAttacks(Color color, Square from, Bitboard targets);
Bitboard generateEnPassantAttacks(Color color, Square from, Square epSquare);
Bitboard generateCastlingAttacks(Color color, Bitboard occupancy, bool canCastleKingside, bool canCastleQueenside);

#endif // ATTACKS_H
