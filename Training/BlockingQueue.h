//
// Created by ludwig on 4/1/21.
//

#ifndef BUSYBEAVER_BLOCKINGQUEUE_H
#define BUSYBEAVER_BLOCKINGQUEUE_H

#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore.h>
#include <queue>

template<typename T>
class BlockingQueue {
    std::queue<T> queue_{};
    mutable std::mutex mutexEmpty{};
    mutable std::mutex mutex{};
    mutable sem_t semaphore{};

    // Moved out of public interface to prevent races between this
    // and pop().
    [[nodiscard]] bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);

        return queue_.empty();
    }

public:
    BlockingQueue() {
        sem_init(&semaphore, 0, 0);
    };
    BlockingQueue(const BlockingQueue<T> &) = delete ;
    BlockingQueue& operator=(const BlockingQueue<T> &) = delete ;

    BlockingQueue(BlockingQueue<T>&& other)  noexcept : BlockingQueue()  {
        std::lock_guard<std::mutex> lock(mutex);
        queue_ = std::move(other.queue_);
    }

    virtual ~BlockingQueue() = default;

    [[nodiscard]] size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue_.size();
    }

    T pop() {
        sem_wait(&semaphore);
        std::lock_guard<std::mutex> lock(mutex);
        T tmp = queue_.front();
        queue_.pop();
        if (queue_.empty()) {
            mutexEmpty.unlock();
        }
        return tmp;
    }

    void push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue_.empty()) {
            mutexEmpty.lock();
        }
        queue_.push(item);
        sem_post(&semaphore);
    }

    void waitUntilEmpty() {
        std::lock_guard<std::mutex> lock(mutexEmpty);
    }
};

#endif //BUSYBEAVER_BLOCKINGQUEUE_H
