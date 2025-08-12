#ifndef EYES_H
#define EYES_H

#include <thread>
#include <atomic>

// Forward declarations (na wszelki wypadek, nie zaszkodzą)
struct AttackerGrid;
struct DefenderGrid;
struct NeutralGrid;

#include "../conAnHea/StrategicGrids.h"

class Eyes {
public:
    Eyes();
    ~Eyes();

    void start();
    void stop();

    void updateGrids();

private:
    void chunkThreadFunc();
    void featureThreadFunc();
    void resonanceThreadFunc();

    std::thread chunkThread;
    std::thread featureThread;
    std::thread resonanceThread;

    std::atomic<bool> running;

    AttackerGrid attackerGrid;
    DefenderGrid defenderGrid;
    NeutralGrid neutralGrid;
};

#endif // EYES_H