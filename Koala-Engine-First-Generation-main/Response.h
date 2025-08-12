//Ten kod ma swoje źródło z repozytorium andersfylling/uci 

#ifndef RESPONSE_H
#define RESPONSE_H

#include "definitions.h" 
#include <iostream>

namespace uci {
void send(std::string res);
void send(std::string command, ::uci::arguments_t args);
}

#endif
