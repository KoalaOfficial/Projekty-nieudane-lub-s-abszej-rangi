#include <array>
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <map>
#include "../conAnHea/Attacks.h"
#include "../conAnHea/constants.h" // Upewnij się, że to zawiera definicje Piece i Color


std::array<uint64_t, 64> knightAttacks = {};
std::array<uint64_t, 64> kingAttacks = {};

// Deklaracje de Bruijn dla 64-bitowych słów
uint64_t deBruijn64 = 0x07776910a2693789ULL;
static const int index64[64] = {
    69, 1, 48, 2, 57, 49, 28, 69,
    61, 58, 50, 42, 38, 29, 17, 4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12, 5,
    63, 47, 56, 66, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 10, 13,
    69, 14, 9, 8, 7, 6, 19, 69
};


// --- Usunięte mapy ataków i związane z nimi deklaracje ---
// using AttacksMap = std::map<std::pair<Color, uint64_t>, int>; // Usunięte
// AttacksMap whiteAttacksMap; // Usunięte
// AttacksMap blackAttacksMap; // Usunięte

// Funkcja do przeskanowania najniższego ustawionego bitu
int bitScanForward(uint64_t bb) {
    if (bb == 0) return -1;
    uint64_t isolated_bit = bb & -bb;
    int index = index64[(isolated_bit * deBruijn64) >> 58];
    return index;
}


void initKnightAttacks() {
    for (int sq = 0; sq < 64; ++sq) {
        uint64_t attacks = 0ULL;
        int r = sq / 8;
        int c = sq % 8;

        const int dr[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
        const int dc[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

        for (int i = 0; i < 8; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                attacks |= 1ULL << (nr * 8 + nc);
            }
        }
        knightAttacks[sq] = attacks;
    }
}

void initKingAttacks() {
    for (int sq = 0; sq < 64; ++sq) {
        uint64_t attacks = 0ULL;
        int r = sq / 8;
        int c = sq % 8;

        const int dr[] = { -1, -1, -1, 0, 1, 1, 1, 0 };
        const int dc[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

        for (int i = 0; i < 8; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                attacks |= 1ULL << (nr * 8 + nc);
            }
        }
        kingAttacks[sq] = attacks;
    }
}

// Funkcja dla wieży
uint64_t generateRookAttacksDB(int sq, uint64_t occupancy) {
    uint64_t attacks = 0ULL;
    int r = sq / 8;
    int c = sq % 8;

    // Ruch w górę
    for (int nr = r + 1; nr < 8; ++nr) {
        int index = nr * 8 + c;
        attacks |= (1ULL << index);
        if (occupancy & (1ULL << index)) break;
    }

    // Ruch w dół
    for (int nr = r - 1; nr >= 0; --nr) {
        int index = nr * 8 + c;
        attacks |= (1ULL << index);
        if (occupancy & (1ULL << index)) break;
    }

    // Ruch w prawo
    for (int nc = c + 1; nc < 8; ++nc) {
        int index = r * 8 + nc;
        attacks |= (1ULL << index);
        if (occupancy & (1ULL << index)) break;
    }

    // Ruch w lewo
    for (int nc = c - 1; nc >= 0; --nc) {
        int index = r * 8 + nc;
        attacks |= (1ULL << index);
        if (occupancy & (1ULL << index)) break;
    }

    return attacks;
}

// Funkcja dla gońca
uint64_t generateBishopAttacksDB(int sq, uint64_t occupancy) {
    uint64_t attacks = 0ULL;
    int r = sq / 8;
    int c = sq % 8;
    const int directions[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };

    for (auto& dir : directions) {
        int dr = dir[0];
        int dc = dir[1];
        int nr = r + dr;
        int nc = c + dc;

        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            int index = nr * 8 + nc;
            attacks |= (1ULL << index);
            if (occupancy & (1ULL << index))
                break;
            nr += dr;
            nc += dc;
        }
    }
    return attacks;
}


// Funkcja generująca ataki pionków (tylko pola ataku, nie ruchy do przodu)
uint64_t generatePawnAttacks(int sq, const Piece& piece) {
    uint64_t attacks = 0ULL;
    int r = sq / 8;
    int c = sq % 8;

    if (piece.color == Color::WHITE) {
        if (r + 1 < 8 && c - 1 >= 0)
            attacks |= 1ULL << ((r + 1) * 8 + c - 1);
        if (r + 1 < 8 && c + 1 < 8)
            attacks |= 1ULL << ((r + 1) * 8 + c + 1);
    } else if (piece.color == Color::BLACK) {
        if (r - 1 >= 0 && c - 1 >= 0)
            attacks |= 1ULL << ((r - 1) * 8 + c - 1);
        if (r - 1 >= 0 && c + 1 < 8)
            attacks |= 1ULL << ((r - 1) * 8 + c + 1);
    }
    return attacks;
}

// Funkcja generująca ataki hetmana
uint64_t generateQueenAttacks(int sq, uint64_t occupancy) {
    return generateRookAttacksDB(sq, occupancy) | generateBishopAttacksDB(sq, occupancy);
}

// Funkcja generująca ataki skoczka (preobliczone)
uint64_t generateKnightAttacks(int sq) {
    return knightAttacks[sq];
}

// Funkcja generująca ataki króla (preobliczone)
uint64_t generateKingAttacks(int sq) {
    return kingAttacks[sq];
}

// Funkcja do generowania ataków dla figury na danym polu
// Zwraca wektor bitboardów ataków (dla większości figur będzie to jeden bitboard)
std::vector<uint64_t> generatePieceAttacks(int fromIndex, const Piece& piece, uint64_t occupancy, uint64_t ownPieces, uint64_t opponentPieces) {
    std::vector<uint64_t> attacks;

    // sprawdzamy, czy figura nie jest pusta
    if (piece.type == PieceType::EMPTY) return attacks;

    // Deklarujemy zmienną 'sq', która jest indeksem pola, z którego generujemy ataki
    int sq = fromIndex;

    // W zależności od typu figury generujemy odpowiednie ataki
    switch (piece.type) {
        case PieceType::PAWN:
            // Dla pionków, generujemy tylko ataki (bicia). Ruch do przodu jest obsługiwany osobno.
            attacks.push_back(generatePawnAttacks(sq, piece));
            break;
        case PieceType::KNIGHT:
            attacks.push_back(generateKnightAttacks(sq));
            break;
        case PieceType::BISHOP:
            attacks.push_back(generateBishopAttacksDB(sq, occupancy));
            break;
        case PieceType::ROOK:
            attacks.push_back(generateRookAttacksDB(sq, occupancy));
            break;
        case PieceType::QUEEN:
            attacks.push_back(generateQueenAttacks(sq, occupancy));
            break;
        case PieceType::KING:
            attacks.push_back(generateKingAttacks(sq));
            break;
        default:
            break;
    }
    return attacks;
}

// Promocja: zwraca bitboard ruchów promocyjnych (na 8 linię)
Bitboard generatePromotionAttacks(Color color, Square from, Bitboard targets) {
    Bitboard attacks = 0ULL;
    int r = from / 8;
    int c = from % 8;
    if (color == Color::WHITE && r == 6) { // pionek na przedostatniej linii
        if (c - 1 >= 0) attacks |= (targets & (1ULL << ((r + 1) * 8 + c - 1)));
        if (c + 1 < 8)  attacks |= (targets & (1ULL << ((r + 1) * 8 + c + 1)));
        attacks |= (targets & (1ULL << ((r + 1) * 8 + c)));
    } else if (color == Color::BLACK && r == 1) {
        if (c - 1 >= 0) attacks |= (targets & (1ULL << ((r - 1) * 8 + c - 1)));
        if (c + 1 < 8)  attacks |= (targets & (1ULL << ((r - 1) * 8 + c + 1)));
        attacks |= (targets & (1ULL << ((r - 1) * 8 + c)));
    }
    return attacks;
}

// En passant: zwraca bitboard ataku en passant
Bitboard generateEnPassantAttacks(Color color, Square from, Square epSquare) {
    Bitboard attacks = 0ULL;
    if (color == Color::WHITE) {
        if (from + 9 == epSquare) attacks |= (1ULL << epSquare);
        if (from + 7 == epSquare) attacks |= (1ULL << epSquare);
    } else {
        if (from - 9 == epSquare) attacks |= (1ULL << epSquare);
        if (from - 7 == epSquare) attacks |= (1ULL << epSquare);
    }
    return attacks;
}

// Roszada: zwraca bitboard dostępnych pól roszady
Bitboard generateCastlingAttacks(Color color, Bitboard occupancy, bool canCastleKingside, bool canCastleQueenside) {
    Bitboard attacks = 0ULL;
    if (color == Color::WHITE) {
        if (canCastleKingside && !(occupancy & 0x0000000000000060ULL))
            attacks |= (1ULL << 6); // g1
        if (canCastleQueenside && !(occupancy & 0x000000000000000EULL))
            attacks |= (1ULL << 2); // c1
    } else {
        if (canCastleKingside && !(occupancy & 0x6000000000000000ULL))
            attacks |= (1ULL << 62); // g8
        if (canCastleQueenside && !(occupancy & 0x0E00000000000000ULL))
            attacks |= (1ULL << 58); // c8
    }
    return attacks;
}