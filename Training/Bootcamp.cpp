//
// Created by ludwig on 4/1/21.
//



#include "Bootcamp.h"

#define MAX_STEPS 100000000

std::uniform_real_distribution<double> Bootcamp::randomDistribution(0.0, 1.0);
std::uniform_int_distribution<int> Bootcamp::randomSymbol(0, 2);
std::uniform_int_distribution<int> Bootcamp::randomDirection(0, 2);
std::uniform_int_distribution<int> Bootcamp::randomState(0, 4);
std::uniform_int_distribution<int> Bootcamp::randomNextState(0, 5);

/*
 * seeds:
 * 1 very good
 * 2
 * 12315417 very good for first steps, but not above 3370
 * 34278 only got to 16 (62 steps) in 50000 rounds
 * */

Bootcamp::Bootcamp(uint8_t numTrainers, const std::vector<BusyBeaver*>& initial) : rng(1) {
    static_assert(std::is_base_of<BaseTuringMachine, BusyBeaver>::value, "BusyBeaver not derived from BaseTuringMachine");
    turingMachines.reserve(generationSize);
    for (auto beaver : initial) {
        turingMachines.emplace_back(beaver, 0);
    }
    if (turingMachines.size() > 0) {
        maxSteps = MAX_STEPS;
    } else {
        fillWithRandomTuringMachines();
        maxSteps = 1000;
    }
    for (int i = 0; i < numTrainers; ++i) {
        auto thread = new std::thread(&Bootcamp::threadFunction, this);
        threads.push_back(thread);
    }
}

BusyBeaver *Bootcamp::getNextQueuedTuringMachine() {
    return queue.pop();
}


[[noreturn]] void Bootcamp::run() {
    uint32_t round = 0;
    auto firstRunTime = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();

    for (auto turingMachine : turingMachines) {
        addToQueue(turingMachine.turingMachine);
    }
    waitUntilTrainersFinish();


    while(true) {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(stop - firstRunTime);
        start = std::chrono::high_resolution_clock::now();

        auto lastBestScore = best.score;
        makeNextGeneration();
        if (best.turingMachine) {
            maxSteps = std::max((uint64_t)1000, std::min((uint64_t)MAX_STEPS, 1000*best.turingMachine->getSteps()));

            std::cout << "Best of round " << round << " (" << duration.count() << "ms): "
                      << best.score
                      << " in " << best.turingMachine->getSteps() << " steps. "
                      << "Now simulating a max of " << maxSteps << " steps. "
                      << "Running for " << totalDuration.count() << "s in total."
                      << std::endl;

        } else {
            std::cout << "No best yet in round " << round << " (" << duration.count() << "ms)"
                      << std::endl;
            maxSteps = 1000;
        }

        if (best.score > lastBestScore) {
            best.turingMachine->save("currentBestBusyBeaver.txt");
        }

        waitUntilTrainersFinish();

        round++;
    }
}

void Bootcamp::fillWithRandomTuringMachines() {

    std::array<char, 3> symbols = {'0', '1', '_'};
    for (size_t i = turingMachines.size(); i < generationSize; ++i) {
        auto newTuringMachine = new BusyBeaver();
        for (int state = 0; state < 5; ++state) {
            for (auto symbol : symbols) {
                newTuringMachine->setTransition(Key(state, symbol), getRandomTransition());
            }
        }
        turingMachines.emplace_back(newTuringMachine, 0);
    }
}


void Bootcamp::makeNextGeneration() {

    for (auto& entry : turingMachines) {
        if (entry.turingMachine->hasAccepted()) {
            entry.score = std::count(entry.turingMachine->getTape().begin(), entry.turingMachine->getTape().end(), '1');
        } else {
            entry.score = 0;
        }
    }

    std::sort(turingMachines.begin(), turingMachines.end(), [](Entry& left, Entry& right)
    {return left.score > right.score
            || (left.score == right.score && left.turingMachine->getSteps() < right.turingMachine->getSteps());});

    if (!turingMachines.empty()) {
        best = turingMachines.front();
    }

    auto first20Percent = turingMachines.begin() + std::max((size_t) 1, size_t(turingMachines.size() * 0.2));
    size_t firstDeathIndex = 0;
    for (auto it = turingMachines.begin(); it != first20Percent; it++) {
        if (!it->turingMachine->hasAccepted()) {
            break;
        }
        firstDeathIndex++;
    }

    if (turingMachines.size() < generationSize) {
        for (size_t i = turingMachines.size(); i < generationSize; ++i) {
            auto newTuringMachine = new BusyBeaver();
            turingMachines.emplace_back(newTuringMachine, 0);
        }
    }
    if (firstDeathIndex > 0) {
        std::uniform_int_distribution<int> randomIndexDistribution(0, firstDeathIndex-1);
        for (size_t i = firstDeathIndex; i < turingMachines.size(); i++) {
            auto randomIndex = randomIndexDistribution(rng);
            auto parent = turingMachines[randomIndex].turingMachine;
            auto newTuringMachine = turingMachines[i].turingMachine;
            newTuringMachine->reset();
            newTuringMachine->copyCommands(*parent);

            //TODO: mutate
            bool recombinated = false;

            if (randomDistribution(rng) <= 0.3) {
                recombinated = true;
                randomIndex = randomIndexDistribution(rng);
                auto secondParent = turingMachines[randomIndex].turingMachine;
                Key randomKey = getRandomKey();
                auto secondParentTransition = secondParent->getTransition(randomKey);
                newTuringMachine->setTransition(randomKey, secondParentTransition);
            }
            if (!recombinated || randomDistribution(rng) <= 0.7) {
                Key randomKey = getRandomKey();
                Transition randomTransition = getRandomTransition();
                newTuringMachine->setTransition(randomKey, randomTransition);
            }

            addToQueue(newTuringMachine);
        }
    } else {
        fillWithRandomTuringMachines();
        for (auto& entry : turingMachines) {
            addToQueue(entry.turingMachine);
        }
    }


}


void Bootcamp::threadSimulatedTuringMachine() {
    std::lock_guard<std::mutex> lock(trainerMutex);
    waitCounter--;
    if (waitCounter == 0) {
        trainersFinishedMutex.unlock();
    }
}


void Bootcamp::waitUntilTrainersFinish() {
    std::lock_guard<std::mutex> lock(trainersFinishedMutex);
}


[[noreturn]] void Bootcamp::threadFunction() {
    while (true) {
        auto turingMachine = getNextQueuedTuringMachine();
        turingMachine->run(maxSteps);
        threadSimulatedTuringMachine();
    }
}



void Bootcamp::addToQueue(BusyBeaver *turingMachine)  {
    std::lock_guard<std::mutex> lock(trainerMutex);
    if (waitCounter == 0) {
        trainersFinishedMutex.lock();
    }
    waitCounter++;
    queue.push(turingMachine);
}


Key Bootcamp::getRandomKey() {
    Key key;
    switch (randomSymbol(rng)) {
        case 0:
            key.symbol = '0';
            break;
        case 1:
            key.symbol = '1';
            break;
        case 2:
            key.symbol = '_';
            break;
    }
    key.state = randomState(rng);
    return key;
}


Transition Bootcamp::getRandomTransition() {

    Transition transition;
    switch (randomSymbol(rng)) {
        case 0:
            transition.writeSymbol = '0';
            break;
        case 1:
            transition.writeSymbol = '1';
            break;
        case 2:
            transition.writeSymbol = '_';
            break;
    }
    switch (randomDirection(rng)) {
        case 0:
            transition.direction = Direction::Left;
            break;
        case 1:
            transition.direction = Direction::Nothing;
            break;
        case 2:
            transition.direction = Direction::Right;
            break;
    }
    transition.nextState = randomNextState(rng);
    if (transition.nextState == 5) {
        transition.nextState = BusyBeaver::finalState;
    }
    return transition;
}
