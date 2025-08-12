#ifndef LISTENER_H
#define LISTENER_H

#include "dismantling.h"

#include <functional>
#include <map>
#include <string>
#include <thread>
#include <mutex>


namespace uci {

// Holds the keys and associated values per UCI input.
typedef std::map<std::string ,std::string>arguments_t;

// The lambda type which is used to add a function listener to an event.
typedef std::function<void(arguments_t)> callback_t;

class Listener {
 private:
  Parser parser; // Dodaj to pole
  bool runListener;
  int lastID;
  bool strict;
  

  std::map<uint8_t, std::map<int, callback_t>> events;
  std::map<int, uint8_t> eventIDs;
  std::mutex eventsMutex;
  std::mutex eventIDsMutex;
  std::thread listener;

 public:
  Listener();
  Listener(bool strict);
  ~Listener();

  bool initiateListener();
  bool joinListener();
  bool setupListener();
  bool joinListenerAndStop();
  void stopListening();

  int addListener(const uint8_t event, const callback_t &func);
  bool hasListener(int listenerID);
  void hasListener(int listenerID, std::function<void(bool exists)> lockedCallback);
  bool removeListener(int listenerID);
  void removeListenerThread(int listenerID);
  void fireEvent(const uint8_t event);
  void fireEvent(const uint8_t event, const uci::arguments_t arguments);
};
}
#endif