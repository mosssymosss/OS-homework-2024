#ifndef PARALLEL_SCHEDULER
#define PARALLEL_SCHEDULER

#include <functional>
#include <vector>
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>


class parallel_scheduler
{
private:
    std::size_t capacity;
    std::vector<pthread_t> workers;
    std::queue<std::function<void()>> que;
    pthread_mutex_t mtx;
    pthread_cond_t cond_var;
    bool stop;
    static void* current_thread(void* arg);
public:

    parallel_scheduler(std::size_t n);
    ~parallel_scheduler();
    void run(std::function<void()> f);
};

#endif
