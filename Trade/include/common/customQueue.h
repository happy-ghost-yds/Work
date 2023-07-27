#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>
#include <iostream>

template <typename T>
class customQueue
{
private:
    std::mutex              m_mutex;
    std::condition_variable m_condition;
    std::deque<T>           m_queue;
    bool m_shutdown = false;
public:
    void push(T const& value)
    {
        {
            std::unique_lock<std::mutex> lock(this->m_mutex);
            m_queue.push_front(value);
        }
        this->m_condition.notify_one();
    }
    T pop()
    {
        std::unique_lock<std::mutex> lock(this->m_mutex);
        this->m_condition.wait(lock, [=]{ return (!this->m_queue.empty() || m_shutdown); });
        if (m_shutdown)
        {
            return T();
        }
        T rc(std::move(this->m_queue.back()));
        this->m_queue.pop_back();
        // std::cout << "qsize=" << m_queue.size() <<"\n";
        return rc;
    }
    bool empty()
    {
        std::unique_lock<std::mutex> lock(this->m_mutex);
        return this->m_queue.empty();
    }
    void shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(this->m_mutex);
            m_shutdown = true;
        }
        this->m_condition.notify_all();
    }

};