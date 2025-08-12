#pragma once

#include "constants.h"
#include "../conAnHea/MoveGen.h"
#include "../conAnHea/BoardUtils.h"

namespace SpecialMoves {

bool isPromotionMove(const Move& move, Color color);
bool isEnPassantMove(const Move& move, Color color, Square epSquare);
bool isCastlingMove(const Move& move, Color color);

}
