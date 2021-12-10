#pragma once


//CREDIT: Written by stackoverflow user "ChewOnThis_Trident", posted here in a question answer : https://stackoverflow.com/questions/15278343/c11-thread-safe-queue
// getSize and clear functions added by Avery Black

#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
    SafeQueue(void)
        : q()
        , m()
        , c()
    {}

    ~SafeQueue(void)
    {}

    // Add an element to the queue.
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty())
        {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    size_t getSize()
    {
        std::lock_guard<std::mutex> lock(m);
        return q.size();
    }

    void clear()
    {
        std::queue<T> emtpy;

        std::lock_guard<std::mutex> lock(m);
        std::swap(q, emtpy);
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};
#endif
