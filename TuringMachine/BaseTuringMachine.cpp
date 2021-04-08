//
// Created by ludwig on 4/4/21.
//
#include <fstream>
#include "BaseTuringMachine.h"

char directionToChar(Direction direction) {
    switch (direction) {
        case Direction::Left:
            return 'l';
        case Direction::Right:
            return 'r';
        case Direction::Nothing:
            return '*';
    }
    return '*';
}

uint32_t BaseTuringMachine::finalState = std::numeric_limits<uint32_t>::max();

BaseTuringMachine::BaseTuringMachine() {
    BaseTuringMachine::reset();
}

void BaseTuringMachine::reset() {
    state = 0;
    tape.clear();
    for (int i = 0; i < 3; ++i) {
        tape.push_back('_');
    }
    head = tape.begin() + 1;
    accepted = false;
}

void BaseTuringMachine::load(const std::filesystem::path &path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();

    std::string line;
    while (std::getline(ss, line, '\n')) {
        Key key;
        Transition transition;
        size_t pos = 0;
        std::string token;

        auto getNext = [&line, &pos]() -> std::string {
            size_t oldPos = pos;
            pos = line.find(' ', oldPos);
            if (pos == std::string::npos) {
                if (oldPos < line.size() && line.back() == '\r') {
                    pos = line.size();
                    return line.substr(oldPos, pos-oldPos-1);
                }
                throw MalformedTuringMachine();
            }
            pos++;
            return line.substr(oldPos, pos-oldPos-1);
        };

        token = getNext();
        key.state = std::stoull(token);

        token = getNext();
        if (token.size() > 1) {
            throw MalformedTuringMachine();
        }
        key.symbol = token.front();

        token = getNext();
        if (token.size() > 1) {
            throw MalformedTuringMachine();
        }
        transition.writeSymbol = token.front();

        token = getNext();
        switch (token.front()) {
            case 'l':
                transition.direction = Direction::Left;
                break;
            case 'r':
                transition.direction = Direction::Right;
                break;
            default:
                transition.direction = Direction::Nothing;
                break;
        }

        token = getNext();
        if (token == "halt") {
            transition.nextState = finalState;
        } else {
            transition.nextState = std::stoull(token);
        }

        setTransition(key, transition);
    }
}

void BaseTuringMachine::save(const std::filesystem::path& path) {
    std::ofstream file(path);
    if (file.is_open()) {
        file << getTransitionString();
        file.close();
    }
}

void BaseTuringMachine::run(uint64_t maxSteps) {
    steps = 0;
    for (; steps < maxSteps; ++steps) {

        auto transition = getNextTransition();
        state = transition.nextState;
        *head = transition.writeSymbol;
        switch (transition.direction) {

            case Direction::Left:
                if (head == tape.begin()) {
                    tape.push_front('_');
                    head = tape.begin();
                } else {
                    head--;
                }
                break;
            case Direction::Right:
                if (head == tape.end()-1) {
                    tape.push_back('_');
                    head = tape.end()-1;
                } else {
                    head++;
                }
                break;
            default:
                break;
        }

        if (state == finalState) {
            accepted = true;
            return;
        }

    }
    accepted = false;
}



#include "BaseTuringMachine.h"
