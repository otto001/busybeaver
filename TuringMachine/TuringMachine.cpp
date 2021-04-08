//
// Created by ludwig on 4/1/21.
//

#include <iostream>
#include "TuringMachine.h"

TuringMachine::TuringMachine() = default;

TuringMachine::TuringMachine(const TuringMachine &turingMachine) : TuringMachine() {
    commands = turingMachine.commands;
}

std::string TuringMachine::getTransitionString() {
    std::stringstream result;

    for (auto & command : commands) {
        result << command.first.state
        << " " << command.first.symbol
        << " " << command.second.writeSymbol
        << " " << directionToChar(command.second.direction)
        << " " << (command.second.nextState == finalState ? "halt" : std::to_string(command.second.nextState))
        << std::endl;
    }
    return result.str();
}

void TuringMachine::setTransition(Key key, Transition transition) {
    commands[key] = transition;
}

const Transition &TuringMachine::getNextTransition() {
    auto key = getCurrentKey();
    return getTransition(key);
}

