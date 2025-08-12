<<<<<<< HEAD
#ifndef ALPHABETA_H
#define ALPHABETA_H

#include <vector>
#include <memory>
#include "../conAnHea/MoveGen.h"
#include "../conAnHea/ChunkEvaluator.h"
#include "../conAnHea/build_chunks.h"
#include "../conAnHea/ChunkFeatureExtractor.h"

struct MoveEvaluation {
    Move move;
    float score;
};

struct TreeNode {
    BoardSnapshot snapshot;
    Move move;
    std::vector<std::unique_ptr<TreeNode>> children;
    float score = 0.0f;

    TreeNode(const BoardSnapshot& snap, const Move& m = Move{})
        : snapshot(snap), move(m) {}
};

struct SearchResult {
    std::unique_ptr<TreeNode> root;
    Move bestMove;
    float score;
    std::vector<MoveEvaluation> evaluatedMoves;
    bool foundMove = false;
};

SearchResult alphaBeta(
    BoardSnapshot snapshot,
    const std::vector<Chunk>& chunksEval,
    int depth,
    float alpha,
    float beta,
    bool maximizingPlayer,
    Color color,
    const ChunkEvaluator& evaluator,
    const ChunkFeatureExtractor& featureExtractor
);

=======
#ifndef ALPHABETA_H
#define ALPHABETA_H

#include <vector>
#include <memory>
#include "../conAnHea/MoveGen.h"
#include "../conAnHea/ChunkEvaluator.h"
#include "../conAnHea/build_chunks.h"
#include "../conAnHea/ChunkFeatureExtractor.h"

struct MoveEvaluation {
    Move move;
    float score;
};

struct TreeNode {
    BoardSnapshot snapshot;
    Move move;
    std::vector<std::unique_ptr<TreeNode>> children;
    float score = 0.0f;

    TreeNode(const BoardSnapshot& snap, const Move& m = Move{})
        : snapshot(snap), move(m) {}
};

struct SearchResult {
    std::unique_ptr<TreeNode> root;
    Move bestMove;
    float score;
    std::vector<MoveEvaluation> evaluatedMoves;
    bool foundMove = false;
};

SearchResult alphaBeta(
    BoardSnapshot snapshot,
    const std::vector<Chunk>& chunksEval,
    int depth,
    float alpha,
    float beta,
    bool maximizingPlayer,
    Color color,
    const ChunkEvaluator& evaluator,
    const ChunkFeatureExtractor& featureExtractor
);

>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
#endif // ALPHABETA_H