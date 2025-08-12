#include "../conAnHea/ChunkMaskGenerator.h"

ChunkMaskSet ChunkMaskGenerator::generateMasks(const std::array<uint64_t, NUM_CHUNKS>& chunkBitboards) const {
    ChunkMaskSet masks;
    for (size_t i = 0; i < NUM_CHUNKS; ++i) {
        uint64_t chunk = chunkBitboards[i];

        // Isolated pawns (no adjacent chunk has pawns)
        bool leftEmpty = (i == 0) || (chunkBitboards[i - 1] == 0);
        bool rightEmpty = (i == NUM_CHUNKS - 1) || (chunkBitboards[i + 1] == 0);
        if (leftEmpty && rightEmpty && chunk != 0) {
            masks.isolatedPawns[i] = chunk;
        }

        // Pawn islands (any chunk with pawns)
        if (chunk != 0) {
            masks.pawnIslands[i] = chunk;
        }

        // Center control (example: central chunks)
        if (i == NUM_CHUNKS / 2 - 1 || i == NUM_CHUNKS / 2) {
            masks.centerControl[i] = chunk;
        }

        // Wing control (edge chunks)
        if (i == 0 || i == 1 || i == NUM_CHUNKS - 2 || i == NUM_CHUNKS - 1) {
            masks.wingControl[i] = chunk;
        }
    }
    return masks;
}