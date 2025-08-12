//Ten kod ma swoje źródło z repozytorium andersfylling/uci 

#include "Parser.h"
#include <sstream>
#include <algorithm>
#include "events.h"

namespace uci {

Parser::Parser() {}

Parser::~Parser() {}

std::pair<uint8_t, arguments_t> Parser::parseInput(const std::string& input) {
  auto command = this->parseInputForCommand(input);
  if (command == ::uci::event::NO_MATCHING_COMMAND) {
    return std::pair<uint8_t, arguments_t>();
  }

  auto arguments = this->parseInputForArguments(input);
  return std::pair<uint8_t, arguments_t>(command, arguments);
}

arguments_t Parser::parseInputForArguments(const std::string& input) {
  std::stringstream request(input);
  std::string command;
  request >> command;
  std::transform(command.begin(), command.end(), command.begin(), ::tolower);

  arguments_t arguments;
  std::string firstWord;
  request >> firstWord;

  const auto cmdEntry = this->commands.find(command);
  if (cmdEntry != this->commands.end()) {
    auto &vecHolder = cmdEntry->second;

    for (int i = 0; i < vecHolder.size(); i++) {
      auto &v = vecHolder.at(i);
      if (v.size() == 0 || v.front() != firstWord) {
        continue;
      }

      arguments[firstWord] = ""; 
      std::string key = firstWord;
      std::string value = "";
      std::string nextWord;
      int j = 1;
      while (request >> nextWord) {
        if (j < v.size() && v.at(j) == nextWord) {
          j += 1;
          arguments[key] = value;
          value = "";
          key = nextWord;
          arguments[key] = "";
          continue;
        }

        if (value == "") {
          value = nextWord;
        } else {
          value += " " + nextWord;
        }
      }
      arguments[key] = value;
      break;
    }
  }

  return arguments;
}

uint8_t Parser::parseInputForCommand(const std::string& input) {
  std::string command = this->getFirstWord(input);
  auto entry = this->commands.find(command);
  uint8_t event = ::uci::event::NO_MATCHING_COMMAND;

  if (entry != this->commands.end()) {
    if (command == "uci") {
      event = ::uci::event::UCI;
    } else if (command == "perft") {
      event = ::uci::event::PERFT;
    } else if (command == "debug") {
      event = ::uci::event::DEBUG;
    } else if (command == "isready") {
      event = ::uci::event::ISREADY;
    } else if (command == "setoption") {
      event = ::uci::event::SETOPTION;
    } else if (command == "register") {
      event = ::uci::event::REGISTER;
    } else if (command == "ucinewgame") {
      event = ::uci::event::UCINEWGAME;
    } else if (command == "position") {
      event = ::uci::event::POSITION;
    } else if (command == "go") {
      event = ::uci::event::GO;
    } else if (command == "stop") {
      event = ::uci::event::STOP;
    } else if (command == "ponderhit") {
      event = ::uci::event::PONDERHIT;
    } else if (command == "quit") {
      event = ::uci::event::QUIT;
    } else if (command == "black") {
      event = ::uci::event::BLACK;
    } else if (command == "white") {
      event = ::uci::event::WHITE;
    }
  }

  return event;
}

std::string Parser::getFirstWord(const std::string& sentence) {
    std::string word = "";
    size_t pos = sentence.find(' ');
    
    if (pos != std::string::npos) {
        word = sentence.substr(0, pos);
    } else {
        word = sentence; // cała linia jest jednym słowem
    }
    
    return word;
}

}
