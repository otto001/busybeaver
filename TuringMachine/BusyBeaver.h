//
// Created by ludwig on 4/4/21.
//

#ifndef BUSYBEAVER_BUSYBEAVER_H
#define BUSYBEAVER_BUSYBEAVER_H


#include <vector>
#include "BaseTuringMachine.h"

class BusyBeaver : public BaseTuringMachine {
public:
    BusyBeaver();
    BusyBeaver(const BusyBeaver& turingMachine);

protected:
    Transition commands[15];
    static uint8_t getKeyCode(const Key& key) {
        return key.state*3 + (key.symbol == '_' ? 2 : (key.symbol - '0'));
    }
    static Key getKeyFromKeyCode(uint8_t keycode) {
        uint8_t state = keycode/3;
        uint8_t symbol = keycode - 3*state;
        symbol = symbol == 2 ? '_' : symbol + '0';
        return Key(state, symbol);
    }
    uint8_t getCurrentKeyCode() {
        char symbol = *head;
        return state*3 + (symbol == '_' ? 2 : (symbol - '0'));
    }
public:


    inline void setTransition(Key key, Transition transition) final {
        commands[getKeyCode(key)] = transition;
    }
    inline const Transition& getTransition(Key key) final {
        return commands[getKeyCode(key)];
    }
    const Transition& getNextTransition() final {
        return commands[getCurrentKeyCode()];
    }

    void copyCommands(const BusyBeaver& other);

    std::string getTransitionString() final;

};


#endif //BUSYBEAVER_BUSYBEAVER_H
