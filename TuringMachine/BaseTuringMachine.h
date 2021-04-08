//
// Created by ludwig on 4/4/21.
//

#ifndef BUSYBEAVER_BASETURINGMACHINE_H
#define BUSYBEAVER_BASETURINGMACHINE_H


#include <list>
#include <deque>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <filesystem>
#include <map>


struct Key {
    size_t state;
    char symbol;

    Key() : state(), symbol() {}
    Key(size_t state, char symbol) : state(state), symbol(symbol) {}

    bool operator<(const Key &p) const
    {
        return state < p.state || (state == p.state && symbol < p.symbol);
    }

    bool operator==(const Key &p) const
    {
        return state == p.state && symbol == p.symbol;
    }

    struct hash
    {
        std::size_t operator() (const Key &key) const
        {
            std::size_t h1 = std::hash<size_t>()(key.state);
            std::size_t h2 = std::hash<char>()(key.symbol);
            return h1 ^ h2;
        }
    };
};



enum class Direction {
    Left,
    Right,
    Nothing
};
char directionToChar(Direction direction);

struct Transition {
    Transition() : nextState(), writeSymbol(), direction(Direction::Nothing) {}

    size_t nextState;
    char writeSymbol;
    Direction direction;
};

struct MalformedTuringMachine : public std::exception
{
    [[nodiscard]] const char * what() const noexcept override
    {
        return "Malformed TuringMachine string";
    }
};

class BaseTuringMachine {
public:
    BaseTuringMachine();
    BaseTuringMachine(BaseTuringMachine&) = delete;

    virtual ~BaseTuringMachine() = default;

    static uint32_t finalState;
protected:

    std::deque<char> tape;
    std::deque<char>::iterator head;
    size_t state{};

    bool accepted = false;
    uint64_t steps = 0;

public:

    [[nodiscard]] virtual bool hasAccepted() const {
        return accepted;
    }
    [[nodiscard]] virtual uint64_t getSteps() const {
        return steps;
    }

    [[nodiscard]] virtual const std::deque<char> &getTape() const {
        return tape;
    }

    Key getCurrentKey() {
        return Key(state, *head);
    }
    virtual void reset();
    virtual void run(uint64_t maxSteps);

    virtual void load(const std::filesystem::path& path);
    virtual void save(const std::filesystem::path& path);
    virtual void setTransition(Key key, Transition transition) = 0;
    virtual const Transition& getTransition(Key key) = 0;
    virtual const Transition& getNextTransition() = 0;

    virtual std::string getTransitionString() = 0;

};


#endif //BUSYBEAVER_BASETURINGMACHINE_H
