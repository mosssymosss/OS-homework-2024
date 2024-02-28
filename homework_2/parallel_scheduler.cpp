
#include "parallel_scheduler.h"
#include <iostream>

parallel_scheduler::parallel_scheduler(std::size_t capacity)
    :capacity(capacity)
    ,stop(false)
{
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cond_var, nullptr);

    workers.resize(capacity);
    for (int i = 0; i < capacity; ++i)
    {
        pthread_create(&workers[i], NULL, &parallel_scheduler::current_thread, this);
    }
}

parallel_scheduler::~parallel_scheduler()
{
	pthread_mutex_lock(&mtx);
    	stop = true;
	pthread_mutex_unlock(&mtx);

	pthread_cond_broadcast(&cond_var);

	for(pthread_t x : workers)
	{
		pthread_join(x, nullptr);
	}	

	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&cond_var);
}

void parallel_scheduler::run(std::function<void()> f)
{
	pthread_mutex_lock(&mtx);
	que.push(f);
	pthread_mutex_unlock(&mtx);

	pthread_cond_signal(&cond_var);
}

void* parallel_scheduler::current_thread(void* arg)
{
	parallel_scheduler* me = static_cast<parallel_scheduler*>(arg);
	
	while(true)
	{
		

		pthread_mutex_lock(&me->mtx);
		while(!me->stop && me->que.empty())
		{
			pthread_cond_wait(&me->cond_var, &me->mtx);
		}
		if(me->stop && me->que.empty())
		{
			pthread_mutex_unlock(&me->mtx);
			return nullptr;
		}

		std::function<void()> func = me->que.front();
		me->que.pop();

		pthread_mutex_unlock(&me->mtx);

		func();
	}

	return nullptr;
}
