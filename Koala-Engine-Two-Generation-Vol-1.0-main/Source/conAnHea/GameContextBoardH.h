#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <vector>
#include <array>
#include "constants.h"

// Struktura do przechowywania właściwości pola
struct FieldProperty {
    int chunkIndex;
    int rowInChunk;
    int colInChunk;
};

class GameContext {
public:
    FieldProperty fieldProperties[BOARD_SIZE * BOARD_SIZE];
    Color currentColor;
    int srcIndex;
    int dstIndex;
    int epSquare = -1;
    
	void createFieldProperties();
  
    GameContext() : currentColor(Color::NONE), srcIndex(-1), dstIndex(-1) {
        createFieldProperties();
    }
    
    
    void setupBoard(std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks);
};


inline void GameContext::createFieldProperties() {
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        int row = i / BOARD_SIZE;
        int col = i % BOARD_SIZE;
        fieldProperties[i].chunkIndex = (row / CHUNK_SIZE) * (BOARD_SIZE / CHUNK_SIZE) + (col / CHUNK_SIZE);
        fieldProperties[i].rowInChunk = row % CHUNK_SIZE;
        fieldProperties[i].colInChunk = col % CHUNK_SIZE;
    }
}

#endif // GAMECONTEXT_H