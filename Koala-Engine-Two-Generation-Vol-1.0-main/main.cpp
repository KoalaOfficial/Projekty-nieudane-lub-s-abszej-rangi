<<<<<<< HEAD
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <array>
#include <locale>
#include <windows.h>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/UCI/Listener.h"
#include "../conAnHea/UCI/dismantling.h"
#include "../conAnHea/UCI/Response.h"
#include "../conAnHea/UCI/events.h"
#include "../conAnHea/UCI/definitions.h"
#include "../conAnHea/MoveGen.h"
#include "../conAnHea/constants.h"
#include "../conAnHea/AlphaBet.h"
#include "../conAnHea/GameContextBoardH.h"
#include "../conAnHea/Attacks.h"
#include "../conAnHea/ChunkMaskGenerator.h"
#include "../conAnHea/StrategicGrids.h"
#include "../conAnHea/build_chunks.h"

using namespace std;

GameContext gameContext;
AttackerGrid attackerGrid;
DefenderGrid defenderGrid;
NeutralGrid neutralGrid;
ChunkMaskGenerator maskGenerator;

void createFieldProperties();
void setColorFromUCI(const std::string& command);
void setupBoard(vector<array<array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks);
void BestMove(BoardUtils& boardUtils, 
              std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks, 
              int srcRow, int srcCol, int dstRow, int dstCol,const Move& move);

void setColorFromUCI(const std::string& command) {
    if (command.find("go") != std::string::npos) {
        gameContext.currentColor = (command.find("w") != std::string::npos) ? Color::WHITE : Color::BLACK;
    }
}

class Config {
public:
    void load(const std::string& filename);
    std::string getOption(const std::string& key) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : "";
    }

private:
    std::unordered_map<std::string, std::string> options; 
};

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open config file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
            options[key] = value;
        }
    }
}

int main() {
    Config config;
    initKnightAttacks();
    initKingAttacks();
    BoardUtils boardUtils;
    BoardSnapshot  boardSnapshot;
    std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>> chunks(NUM_CHUNKS);

    setlocale(LC_ALL, "pl-PL");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    uci::Listener listener;

    listener.addListener(uci::event::UCI, [&](uci::arguments_t args) {
        cout << "id name Koala Engine" << endl
             << "id author Franciszek Lassak" << endl
             << "uciok" << endl;
    });

    listener.addListener(uci::event::ISREADY, [](uci::arguments_t args) {
        cout << "readyok" << endl;
    });

   listener.addListener(uci::event::GO, [&](uci::arguments_t args) {
        if (!args.empty()) {
            setColorFromUCI(args[0]);
            config.load("Config.ini");
            uint64_t hash = boardUtils.getCurrentHash();

            auto bitboards = boardUtils.getBothBitboards();
            boardSnapshot = createBoardSnapshot(bitboards);
            fillBoardSnapshot(boardSnapshot, chunks); // tylko wywołaj, NIE przypisuj!

			
            int depth = std::stoi(config.getOption("depth"));
            float alpha = -std::numeric_limits<float>::infinity();
            float beta  = std::numeric_limits<float>::infinity();
            bool maximizingPlayer = (gameContext.currentColor == Color::WHITE);

          ChunkFeatureExtractor featureExtractor(attackerGrid, defenderGrid, neutralGrid);
ChunkMaskGenerator maskGenerator;
auto evaluator = ChunkEvaluator(
    std::make_unique<ChunkFeatureExtractor>(attackerGrid, defenderGrid, neutralGrid),
    std::make_unique<ChunkMaskGenerator>()
);
// chunksEval: tylko do alphaBeta i evaluacji!
std::vector<Chunk> chunksEval = buildChunks(boardSnapshot, featureExtractor);

SearchResult result = alphaBeta(
    boardSnapshot,
    chunksEval, // <--- chunk-i z maskami i featurami!
    depth,
    alpha,
    beta,
    maximizingPlayer,
    gameContext.currentColor,
    evaluator,         // <--- POPRAWNA NAZWA
    featureExtractor
);
            if (result.foundMove) {
                gameContext.srcIndex = result.bestMove.from;
                gameContext.dstIndex = result.bestMove.to;

                int srcRow = gameContext.srcIndex / BOARD_SIZE;
                int srcCol = gameContext.srcIndex % BOARD_SIZE;
                int dstRow = gameContext.dstIndex / BOARD_SIZE;
                int dstCol = gameContext.dstIndex % BOARD_SIZE;

                BestMove(boardUtils, chunks, srcRow, srcCol, dstRow, dstCol, result.bestMove);
            }
        }
    });

    listener.addListener(uci::event::POSITION, [&](uci::arguments_t args) {
        gameContext.createFieldProperties();
        setupBoard(chunks);
        boardUtils.initialize(chunks);
    });

    listener.addListener(uci::event::QUIT, [&](uci::arguments_t args) {
        listener.stopListening();
        cout << "Program przestał nasłuchiwać!" << endl;
    });

    listener.setupListener();
    return 0;
=======
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <array>
#include <locale>
#include <windows.h>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "../conAnHea/BoardUtils.h"
#include "../conAnHea/UCI/Listener.h"
#include "../conAnHea/UCI/dismantling.h"
#include "../conAnHea/UCI/Response.h"
#include "../conAnHea/UCI/events.h"
#include "../conAnHea/UCI/definitions.h"
#include "../conAnHea/MoveGen.h"
#include "../conAnHea/constants.h"
#include "../conAnHea/AlphaBet.h"
#include "../conAnHea/GameContextBoardH.h"
#include "../conAnHea/Attacks.h"
#include "../conAnHea/ChunkMaskGenerator.h"
#include "../conAnHea/StrategicGrids.h"
#include "../conAnHea/build_chunks.h"

using namespace std;

GameContext gameContext;
AttackerGrid attackerGrid;
DefenderGrid defenderGrid;
NeutralGrid neutralGrid;
ChunkMaskGenerator maskGenerator;

void createFieldProperties();
void setColorFromUCI(const std::string& command);
void setupBoard(vector<array<array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks);
void BestMove(BoardUtils& boardUtils, 
              std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>>& chunks, 
              int srcRow, int srcCol, int dstRow, int dstCol,const Move& move);

void setColorFromUCI(const std::string& command) {
    if (command.find("go") != std::string::npos) {
        gameContext.currentColor = (command.find("w") != std::string::npos) ? Color::WHITE : Color::BLACK;
    }
}

class Config {
public:
    void load(const std::string& filename);
    std::string getOption(const std::string& key) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : "";
    }

private:
    std::unordered_map<std::string, std::string> options; 
};

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open config file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
            options[key] = value;
        }
    }
}

int main() {
    Config config;
    initKnightAttacks();
    initKingAttacks();
    BoardUtils boardUtils;
    BoardSnapshot  boardSnapshot;
    std::vector<std::array<std::array<Piece, CHUNK_SIZE>, CHUNK_SIZE>> chunks(NUM_CHUNKS);

    setlocale(LC_ALL, "pl-PL");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    uci::Listener listener;

    listener.addListener(uci::event::UCI, [&](uci::arguments_t args) {
        cout << "id name Koala Engine" << endl
             << "id author Franciszek Lassak" << endl
             << "uciok" << endl;
    });

    listener.addListener(uci::event::ISREADY, [](uci::arguments_t args) {
        cout << "readyok" << endl;
    });

   listener.addListener(uci::event::GO, [&](uci::arguments_t args) {
        if (!args.empty()) {
            setColorFromUCI(args[0]);
            config.load("Config.ini");
            uint64_t hash = boardUtils.getCurrentHash();

            auto bitboards = boardUtils.getBothBitboards();
            boardSnapshot = createBoardSnapshot(bitboards);
            fillBoardSnapshot(boardSnapshot, chunks); // tylko wywołaj, NIE przypisuj!

			
            int depth = std::stoi(config.getOption("depth"));
            float alpha = -std::numeric_limits<float>::infinity();
            float beta  = std::numeric_limits<float>::infinity();
            bool maximizingPlayer = (gameContext.currentColor == Color::WHITE);

          ChunkFeatureExtractor featureExtractor(attackerGrid, defenderGrid, neutralGrid);
ChunkMaskGenerator maskGenerator;
auto evaluator = ChunkEvaluator(
    std::make_unique<ChunkFeatureExtractor>(attackerGrid, defenderGrid, neutralGrid),
    std::make_unique<ChunkMaskGenerator>()
);
// chunksEval: tylko do alphaBeta i evaluacji!
std::vector<Chunk> chunksEval = buildChunks(boardSnapshot, featureExtractor);

SearchResult result = alphaBeta(
    boardSnapshot,
    chunksEval, // <--- chunk-i z maskami i featurami!
    depth,
    alpha,
    beta,
    maximizingPlayer,
    gameContext.currentColor,
    evaluator,         // <--- POPRAWNA NAZWA
    featureExtractor
);
            if (result.foundMove) {
                gameContext.srcIndex = result.bestMove.from;
                gameContext.dstIndex = result.bestMove.to;

                int srcRow = gameContext.srcIndex / BOARD_SIZE;
                int srcCol = gameContext.srcIndex % BOARD_SIZE;
                int dstRow = gameContext.dstIndex / BOARD_SIZE;
                int dstCol = gameContext.dstIndex % BOARD_SIZE;

                BestMove(boardUtils, chunks, srcRow, srcCol, dstRow, dstCol, result.bestMove);
            }
        }
    });

    listener.addListener(uci::event::POSITION, [&](uci::arguments_t args) {
        gameContext.createFieldProperties();
        setupBoard(chunks);
        boardUtils.initialize(chunks);
    });

    listener.addListener(uci::event::QUIT, [&](uci::arguments_t args) {
        listener.stopListening();
        cout << "Program przestał nasłuchiwać!" << endl;
    });

    listener.setupListener();
    return 0;
>>>>>>> b8d902617d8c303d49069cf9cd3287a41f899c3e
}