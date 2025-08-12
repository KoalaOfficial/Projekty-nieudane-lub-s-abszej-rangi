#ifndef CHUNKMASKGENERATOR_H
#define CHUNKMASKGENERATOR_H

#include <cstdint>
#include <array>
#include "constants.h"

struct ChunkMaskSet {
    std::array<uint64_t, NUM_CHUNKS> isolatedPawns{};
    std::array<uint64_t, NUM_CHUNKS> pawnIslands{};
    std::array<uint64_t, NUM_CHUNKS> centerControl{};
    std::array<uint64_t, NUM_CHUNKS> wingControl{};
};

class ChunkMaskGenerator {
public:
    ChunkMaskSet generateMasks(const std::array<uint64_t, NUM_CHUNKS>& chunkBitboards) const;
};

#endif // CHUNKMASKGENERATOR_H