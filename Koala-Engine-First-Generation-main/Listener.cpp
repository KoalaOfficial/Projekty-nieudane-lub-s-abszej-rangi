//Ten kod ma swoje źródło z repozytorium andersfylling/uci.

#include "Listener.h"
#include "events.h"
#include "definitions.h"
#include "Parser.h"
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <mutex>

namespace uci {

Listener::Listener()
    : runListener(false),
      lastID(2),
      strict(false),
      parser() {} // Parser jest teraz poprawnie zainicjalizowany

Listener::Listener(bool strict)
    : runListener(false),
      lastID(2),
      strict(strict),
      parser() {} // Parser jest teraz poprawnie zainicjalizowany
      
Listener::~Listener() {
    if (this->listener.joinable()) {
        this->listener.join();
    }
}


bool Listener::initiateListener() {
    this->runListener = true;

    this->listener = std::thread([this] {
        std::string line;
        while (this->runListener && std::getline(std::cin, line)) {
            if (line.empty()) {
                continue;
            }

            auto event = this->parser.parseInputForCommand(line); // Teraz parser jest poprawnie zdefiniowany

            if (this->strict && event == uci::event::NO_MATCHING_COMMAND) {
                continue;
            }

            auto args = this->parser.parseInputForArguments(line); // Teraz parser jest poprawnie zdefiniowany
            this->fireEvent(event, args);
        }
    });

    return true;
}

bool Listener::joinListener() {
    if (this->listener.joinable()) {
        this->listener.join();
        return true;
    }
    return false;
}

bool Listener::setupListener() {
    this->initiateListener();
    return this->joinListener();
}

bool Listener::joinListenerAndStop() {
    this->stopListening();
    return this->joinListener();
}

void Listener::stopListening() {
    this->runListener = false;
}

int Listener::addListener(const uint8_t event, const callback_t &func) {
    const int id = ++this->lastID;

    std::lock_guard<std::mutex> lock(this->eventsMutex);

    if (this->events.count(event) == 0) {
        this->events[event] = std::map<int, callback_t>();
    }

    this->events[event][id] = func;
    this->eventIDs[id] = event;

    return id;
}

bool Listener::hasListener(int listenerID) {
    std::lock_guard<std::mutex> lock(this->eventIDsMutex);
    return this->eventIDs.count(listenerID) > 0;
}

void Listener::hasListener(int listenerID, std::function<void(bool exists)> lockedCallback) {
    std::lock(this->eventIDsMutex, this->eventsMutex);
    std::lock_guard<std::mutex> lock1(this->eventIDsMutex, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(this->eventsMutex, std::adopt_lock);

    bool exists = this->hasListener(listenerID);
    lockedCallback(exists);
}

bool Listener::removeListener(int listenerID) {
    std::lock(this->eventIDsMutex, this->eventsMutex);
    std::lock_guard<std::mutex> lock1(this->eventIDsMutex, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(this->eventsMutex, std::adopt_lock);

    if (!this->hasListener(listenerID)) {
        return true;
    }

    auto eventID = this->eventIDs[listenerID];
    this->events[eventID].erase(listenerID);
    this->eventIDs.erase(listenerID);

    return !this->hasListener(listenerID);
}

void Listener::removeListenerThread(int listenerID) {
    this->hasListener(listenerID, [&](bool exists) {
        if (exists) {
            this->removeListener(listenerID);
        }
    });
}

void Listener::fireEvent(const uint8_t event) {
    this->fireEvent(event, uci::arguments_t());
}

void Listener::fireEvent(const uint8_t event, const uci::arguments_t arguments) {
    std::lock_guard<std::mutex> lock(this->eventsMutex);

    auto entry = this->events.find(event);
    if (entry == this->events.end()) {
        return;
    }

    for (const auto &observerEntry : entry->second) {
        observerEntry.second(arguments);
    }
}

} // namespace uci
