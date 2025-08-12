#include "../conAnHea/SpecialMoves.h"
#include <cmath>

namespace SpecialMoves {

  bool isPromotionMove(const Move& move, Color color) {
    int fromRank = move.from / 8;
    int toRank = move.to / 8;
    return (color == Color::WHITE && fromRank == 6 && toRank == 7) ||
           (color == Color::BLACK && fromRank == 1 && toRank == 0);
}

bool isEnPassantMove(const Move& move, Color color, Square epSquare) {
    return (move.to == epSquare);
}

bool isCastlingMove(const Move& move, Color color) {
    // Przykład: e1->g1 lub e1->c1 dla białych, e8->g8/c8 dla czarnych
    if (color == Color::WHITE && move.from == 4 && (move.to == 6 || move.to == 2)) return true;
    if (color == Color::BLACK && move.from == 60 && (move.to == 62 || move.to == 58)) return true;
    return false;
}
}
