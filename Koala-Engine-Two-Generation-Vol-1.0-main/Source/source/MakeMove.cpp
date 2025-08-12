#include "../conAnHea/constants.h"
#include "../conAnHea/GameContextBoardH.h"
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/MoveGen.h"   // <- tu jest definicja struktury Move!
#include <iostream>
#include <vector>
#include <array>

extern GameContext gameContext;

// Funkcja do aktualizacji ruchu
void BestMove(BoardUtils& boardUtils, 
              std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks, 
              int srcRow, int srcCol, int dstRow, int dstCol,
              const Move& move)
{
    int srcChunkIndex = gameContext.fieldProperties[srcRow * BOARD_SIZE + srcCol].chunkIndex;
    int dstChunkIndex = gameContext.fieldProperties[dstRow * BOARD_SIZE + dstCol].chunkIndex;

    Piece& pieceToMove = chunks[srcChunkIndex][srcRow % CHUNK_SIZE][srcCol % CHUNK_SIZE];

    // Obsługa promocji
    if (move.promotionPiece != PieceType::EMPTY) {
        chunks[srcChunkIndex][srcRow % CHUNK_SIZE][srcCol % CHUNK_SIZE] = EMPTY_PIECE;
        chunks[dstChunkIndex][dstRow % CHUNK_SIZE][dstCol % CHUNK_SIZE] =
            Piece{pieceToMove.color, move.promotionPiece};
        // TODO: Aktualizuj bitboardy i hash przy promocji jeśli wymagane!
        return;
    }

    // Obsługa en passant
    if (pieceToMove.type == PieceType::PAWN && move.to == gameContext.epSquare) {
        int capturedPawnRow = (pieceToMove.color == Color::WHITE) ? dstRow - 1 : dstRow + 1;
        int capturedPawnChunk = gameContext.fieldProperties[capturedPawnRow * BOARD_SIZE + dstCol].chunkIndex;
        chunks[capturedPawnChunk][capturedPawnRow % CHUNK_SIZE][dstCol % CHUNK_SIZE] = EMPTY_PIECE;

        // Aktualizuj bitboard przeciwnika
        int capturedPawnPos = capturedPawnChunk * CHUNK_SIZE * CHUNK_SIZE +
                              (capturedPawnRow % CHUNK_SIZE) * CHUNK_SIZE +
                              (dstCol % CHUNK_SIZE);
        uint64_t whiteBitboard = boardUtils.getWhiteBitboard();
        uint64_t blackBitboard = boardUtils.getBlackBitboard();
        if (pieceToMove.color == Color::WHITE) {
            blackBitboard &= ~(1ULL << capturedPawnPos);
            boardUtils.setBlackBitboard(blackBitboard);
        } else {
            whiteBitboard &= ~(1ULL << capturedPawnPos);
            boardUtils.setWhiteBitboard(whiteBitboard);
        }
    }

    // Obsługa roszady (przestawienie wieży)
    if (pieceToMove.type == PieceType::KING && abs(move.to - move.from) == 2) {
        Color kingColor = pieceToMove.color;
        if (move.to == 6) { // Białe królewskie
            chunks[0][0][7] = EMPTY_PIECE;
            chunks[0][0][5] = Piece{kingColor, PieceType::ROOK};
        } else if (move.to == 2) { // Białe hetmańskie
            chunks[0][0][0] = EMPTY_PIECE;
            chunks[0][0][3] = Piece{kingColor, PieceType::ROOK};
        } else if (move.to == 62) { // Czarne królewskie
            chunks[7][7][7] = EMPTY_PIECE;
            chunks[7][7][5] = Piece{kingColor, PieceType::ROOK};
        } else if (move.to == 58) { // Czarne hetmańskie
            chunks[7][7][0] = EMPTY_PIECE;
            chunks[7][7][3] = Piece{kingColor, PieceType::ROOK};
        }
        // TODO: Aktualizuj bitboardy i hash dla wieży przy roszadzie jeśli wymagane! // do zrobienia !!
    }

    // Przed wykonaniem ruchu sprawdź, czy na docelowym polu stoi figura przeciwnika i zaktualizuj jej bitboard
    Piece& dstPiece = chunks[dstChunkIndex][dstRow % CHUNK_SIZE][dstCol % CHUNK_SIZE];
    int srcPosition = srcChunkIndex * CHUNK_SIZE * CHUNK_SIZE + (srcRow % CHUNK_SIZE) * CHUNK_SIZE + (srcCol % CHUNK_SIZE);
    int dstPosition = dstChunkIndex * CHUNK_SIZE * CHUNK_SIZE + (dstRow % CHUNK_SIZE) * CHUNK_SIZE + (dstCol % CHUNK_SIZE);

    uint64_t whiteBitboard = boardUtils.getWhiteBitboard();
    uint64_t blackBitboard = boardUtils.getBlackBitboard();
    if (dstPiece.color != pieceToMove.color && dstPiece.type != PieceType::EMPTY) {
        if (dstPiece.color == Color::WHITE) {
            whiteBitboard &= ~(1ULL << dstPosition);
            boardUtils.setWhiteBitboard(whiteBitboard);
        } else if (dstPiece.color == Color::BLACK) {
            blackBitboard &= ~(1ULL << dstPosition);
            boardUtils.setBlackBitboard(blackBitboard);
        }
    }

    // Wykonaj ruch na planszy
    chunks[srcChunkIndex][srcRow % CHUNK_SIZE][srcCol % CHUNK_SIZE] = EMPTY_PIECE;
    chunks[dstChunkIndex][dstRow % CHUNK_SIZE][dstCol % CHUNK_SIZE] = pieceToMove;

    // Aktualizacja hasha
    uint64_t currentHash = boardUtils.getCurrentHash();
    auto& zobristTable = boardUtils.getZobristTable();
    currentHash ^= zobristTable[static_cast<int>(pieceToMove.color)][static_cast<int>(pieceToMove.type)][srcPosition];
    currentHash ^= zobristTable[static_cast<int>(pieceToMove.color)][static_cast<int>(pieceToMove.type)][dstPosition];
    boardUtils.setCurrentHash(currentHash);

    // Aktualizacja bitboardów
    if (pieceToMove.color == Color::WHITE) {
        whiteBitboard = (whiteBitboard & ~(1ULL << srcPosition)) | (1ULL << dstPosition);
        boardUtils.setWhiteBitboard(whiteBitboard);
    } else if (pieceToMove.color == Color::BLACK) {
        blackBitboard = (blackBitboard & ~(1ULL << srcPosition)) | (1ULL << dstPosition);
        boardUtils.setBlackBitboard(blackBitboard);
    }
}