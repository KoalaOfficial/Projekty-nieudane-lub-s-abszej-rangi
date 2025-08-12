<<<<<<< HEAD
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <array>
#include <iostream>
#include <unordered_map>

typedef uint64_t Bitboard;
typedef int Square;
// Rozmiar planszy i kawałków
constexpr int BOARD_SIZE = 8;
constexpr int CHUNK_SIZE = 2;
constexpr int NUM_CHUNKS = (BOARD_SIZE / CHUNK_SIZE) * (BOARD_SIZE / CHUNK_SIZE); //16

// Definiujemy kolory szachowe
enum class Color {
    WHITE,
    BLACK,
    NONE // Dodano, aby rozwiązać problemy z brakiem 'NONE'
};


// Typ wyliczeniowy do zdefiniowania rodzajów figur
enum class PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    EMPTY // Dla pustego pola
};

// Dodajemy strukturę do przechowywania ataków
struct AttackMap {
    std::unordered_map<uint64_t, uint64_t> attacks; // hashmap do przechowywania ataków i ich hashów
};

// Struktura reprezentująca figurę
struct Piece {
    Color color;
    PieceType type;
    
    constexpr Piece() : color(Color::NONE), type(PieceType::EMPTY) {}
    constexpr Piece(Color c, PieceType t) : color(c), type(t) {}
};


// Definiowanie figur
constexpr Piece WHITE_PAWN(Color::WHITE, PieceType::PAWN);
constexpr Piece WHITE_KNIGHT(Color::WHITE, PieceType::KNIGHT);                                      
constexpr Piece WHITE_BISHOP(Color::WHITE, PieceType::BISHOP);
constexpr Piece WHITE_ROOK(Color::WHITE, PieceType::ROOK);
constexpr Piece WHITE_QUEEN(Color::WHITE, PieceType::QUEEN);
constexpr Piece WHITE_KING(Color::WHITE, PieceType::KING);

constexpr Piece BLACK_PAWN(Color::BLACK, PieceType::PAWN);
constexpr Piece BLACK_KNIGHT(Color::BLACK, PieceType::KNIGHT);
constexpr Piece BLACK_BISHOP(Color::BLACK, PieceType::BISHOP);
constexpr Piece BLACK_ROOK(Color::BLACK, PieceType::ROOK);
constexpr Piece BLACK_QUEEN(Color::BLACK, PieceType::QUEEN);
constexpr Piece BLACK_KING(Color::BLACK, PieceType::KING);

// Pusty (brak figury)
constexpr Piece EMPTY_PIECE(Color::NONE, PieceType::EMPTY);

constexpr int NUM_PIECE_TYPES = 6; // PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
constexpr int NUM_COLORS = 2; // WHITE, BLACK

extern Color currentColor; // Deklaracja, nie definicja

=======
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <array>
#include <iostream>
#include <unordered_map>

typedef uint64_t Bitboard;
typedef int Square;
// Rozmiar planszy i kawałków
constexpr int BOARD_SIZE = 8;
constexpr int CHUNK_SIZE = 2;
constexpr int NUM_CHUNKS = (BOARD_SIZE / CHUNK_SIZE) * (BOARD_SIZE / CHUNK_SIZE); //16

// Definiujemy kolory szachowe
enum class Color {
    WHITE,
    BLACK,
    NONE // Dodano, aby rozwiązać problemy z brakiem 'NONE'
};


// Typ wyliczeniowy do zdefiniowania rodzajów figur
enum class PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    EMPTY // Dla pustego pola
};

// Dodajemy strukturę do przechowywania ataków
struct AttackMap {
    std::unordered_map<uint64_t, uint64_t> attacks; // hashmap do przechowywania ataków i ich hashów
};

// Struktura reprezentująca figurę
struct Piece {
    Color color;
    PieceType type;
    
    constexpr Piece() : color(Color::NONE), type(PieceType::EMPTY) {}
    constexpr Piece(Color c, PieceType t) : color(c), type(t) {}
};


// Definiowanie figur
constexpr Piece WHITE_PAWN(Color::WHITE, PieceType::PAWN);
constexpr Piece WHITE_KNIGHT(Color::WHITE, PieceType::KNIGHT);                                      
constexpr Piece WHITE_BISHOP(Color::WHITE, PieceType::BISHOP);
constexpr Piece WHITE_ROOK(Color::WHITE, PieceType::ROOK);
constexpr Piece WHITE_QUEEN(Color::WHITE, PieceType::QUEEN);
constexpr Piece WHITE_KING(Color::WHITE, PieceType::KING);

constexpr Piece BLACK_PAWN(Color::BLACK, PieceType::PAWN);
constexpr Piece BLACK_KNIGHT(Color::BLACK, PieceType::KNIGHT);
constexpr Piece BLACK_BISHOP(Color::BLACK, PieceType::BISHOP);
constexpr Piece BLACK_ROOK(Color::BLACK, PieceType::ROOK);
constexpr Piece BLACK_QUEEN(Color::BLACK, PieceType::QUEEN);
constexpr Piece BLACK_KING(Color::BLACK, PieceType::KING);

// Pusty (brak figury)
constexpr Piece EMPTY_PIECE(Color::NONE, PieceType::EMPTY);

constexpr int NUM_PIECE_TYPES = 6; // PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
constexpr int NUM_COLORS = 2; // WHITE, BLACK

extern Color currentColor; // Deklaracja, nie definicja

>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
#endif // CONSTANTS_H