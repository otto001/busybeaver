#include <iostream>
#include "TuringMachine/TuringMachine.h"
#include "TuringMachine/BusyBeaver.h"
#include "Training/Bootcamp.h"

int main() {

    auto tm = new BusyBeaver();
    tm->load("TM.txt");

    auto start = std::chrono::high_resolution_clock::now();
    tm->run(100000000);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    uint32_t score = std::count(tm->getTape().begin(), tm->getTape().end(), '1');

    std::cout << "It took " << duration.count() << "ms to run the loaded TuringMachine for "
              << tm->getSteps() << " steps. "
              << "The Tape reached a size of " << tm->getTape().size() << " and contained " << score << " 1s."
              << std::endl;

    tm->reset();
    Bootcamp bootcamp(16, {tm});

    bootcamp.run();


    return 0;
}
