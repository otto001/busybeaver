//
// Created by ludwig on 4/1/21.
//

#ifndef BUSYBEAVER_TURINGMACHINE_H
#define BUSYBEAVER_TURINGMACHINE_H


#include "BaseTuringMachine.h"


class TuringMachine : public BaseTuringMachine {
public:
    TuringMachine();
    TuringMachine(const TuringMachine& turingMachine);

protected:

    std::unordered_map<Key, Transition, Key::hash> commands;
public:

    inline void setTransition(Key key, Transition transition) override;
    inline const Transition& getTransition(Key key) override {
        return commands[key];
    }
    const Transition& getNextTransition() override;

    std::string getTransitionString() override;

};


#endif //BUSYBEAVER_TURINGMACHINE_H
