//
// Created by ludwig on 4/1/21.
//

#ifndef BUSYBEAVER_BOOTCAMP_H
#define BUSYBEAVER_BOOTCAMP_H

#include <cstdint>
#include <vector>
#include <random>
#include <mutex>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include "BlockingQueue.h"
#include "../TuringMachine/BaseTuringMachine.h"
#include "../TuringMachine/BusyBeaver.h"


class Bootcamp {
public:
    explicit Bootcamp(uint8_t numTrainers, const std::vector<BusyBeaver*>& initial);

protected:
    struct Entry {
        Entry() = default;
        Entry(BusyBeaver* turingMachine, uint32_t score) : turingMachine(turingMachine), score(score) {};

        BusyBeaver* turingMachine;
        uint32_t score{};
    };


    std::vector<Entry> turingMachines;
    Entry best;

    std::vector<std::thread*> threads;
    BlockingQueue<BusyBeaver*> queue;
    std::mutex trainerMutex;
    std::mutex trainersFinishedMutex;
    size_t waitCounter = 0;

    [[noreturn]] void threadFunction();
    void waitUntilTrainersFinish();

    void addToQueue(BusyBeaver* turingMachine);

    uint64_t maxSteps = 0;

public:
    void threadSimulatedTuringMachine();
protected:

    void makeNextGeneration();
    void fillWithRandomTuringMachines();

    uint16_t generationSize = 500;

    std::mt19937 rng;

    Key getRandomKey();
    Transition getRandomTransition();

    static std::uniform_real_distribution<double> randomDistribution;
    static std::uniform_int_distribution<int> randomSymbol;
    static std::uniform_int_distribution<int> randomDirection;
    static std::uniform_int_distribution<int> randomState;
    static std::uniform_int_distribution<int> randomNextState;

public:
    BusyBeaver* getNextQueuedTuringMachine();

    [[noreturn]] void run();
};


#endif //BUSYBEAVER_BOOTCAMP_H
