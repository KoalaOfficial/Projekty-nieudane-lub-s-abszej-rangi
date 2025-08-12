#ifndef PARSER_H
#define PARSER_H

#include "definitions.h" // Upewnij się, że ścieżka jest poprawna

#include <vector>
#include <map>
#include <string>

namespace uci {
using arguments_t = std::map<std::string, std::string>; // Dodano definicję arguments_t

class Parser {
 private:
  const std::map<std::string, std::vector<std::vector<std::string>>> commands = {
      {"perft", {
          {"depth", "FEN"},
          {"depth"},
          {"FEN"},
          {}
      }},
      {"uci", {
          {}
      }},
      {"debug", {
          {"on"},
          {"off"}
      }},
      {"isready", {
          {}
      }},
      {"setoption", {
          {"name", "value"}
      }},
      {"register", {
          {"later"},
          {"name", "code"}
      }},
      {"ucinewgame", {
          {}
      }},
      {"position", {
          {"fen", "moves"},
          {"startpos", "moves"}
      }},
      {"go", {
          {"searchmoves"},
          {"ponder"},
          {"wtime"},
          {"btime"},
          {"winc"},
          {"binc"},
          {"movestogo"},
          {"depth"},
          {"nodes"},
          {"mate"},
          {"movetime"},
          {"infinite"}
      }},
      {"stop", {
          {}
      }},
      {"ponderhit", {
          {}
      }},
      {"quit", {
          {}
      }},
      {"black", {
          {}
      }},
      {"white", {
          {}
      }}
  };

 public:
  Parser();
  ~Parser();

  uint8_t parseInputForCommand(const std::string& input);
  arguments_t parseInputForArguments(const std::string& input);
  std::pair<uint8_t, arguments_t> parseInput(const std::string& input);

  std::string getFirstWord(const std::string& sentence);
};

}

#endif
