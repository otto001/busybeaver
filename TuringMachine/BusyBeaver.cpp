//
// Created by ludwig on 4/4/21.
//

#include <iostream>
#include "BusyBeaver.h"


BusyBeaver::BusyBeaver() = default;

BusyBeaver::BusyBeaver(const BusyBeaver &turingMachine) {
    copyCommands(turingMachine);
}

std::string BusyBeaver::getTransitionString() {
    std::stringstream result;

    for (uint8_t keycode = 0; keycode < 15; keycode++) {
        Key key = getKeyFromKeyCode(keycode);
        Transition transition = commands[keycode];

        result << key.state
               << " " << key.symbol
               << " " << transition.writeSymbol
               << " " << directionToChar(transition.direction)
               << " " << (transition.nextState == finalState ? "halt" : std::to_string(transition.nextState))
               << std::endl;
    }
    return result.str();
}

void BusyBeaver::copyCommands(const BusyBeaver &other) {
    std::copy(std::begin(other.commands), std::end(other.commands), std::begin(commands));
}
