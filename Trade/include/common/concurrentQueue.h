/*
    Thread_safe queue with conditional variable
*/
#include<queue>
#include<chrono>
#include<mutex>

template<typename dataType>
class CConcurrentQueue
{
private:
    std::queue<dataType> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_forceExit = false;

public:
    void push(dataType const& data);
    bool isEmpty() const;
    bool pop(dataType& popped_value);
    bool waitPop(dataType& popped_value);
    bool timedWaitPop(dataType& popped_value, long ms = 1000);
    int size();
    void clear();
    bool isExit() const;
};