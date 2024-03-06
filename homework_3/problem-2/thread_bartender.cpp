#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t tobacco_sem;
sem_t paper_sem;
sem_t match_sem;
sem_t bartender_sem;

void* smoker_t(void* arg)
{
	
	while(true)
	{
		sem_wait(&tobacco_sem);
		std::cout<<'T'<<std::endl;
		sem_post(&bartender_sem);
	}

	return nullptr;
}

void* smoker_p(void* arg)
{
	while(true)
	{
		sem_wait(&paper_sem);
		std::cout<<'P'<<std::endl;
		sem_post(&bartender_sem);
	}
	return nullptr;
}


void* smoker_m(void* arg)
{
	while(true)
	{
		sem_wait(&match_sem);
		std::cout<<'M'<<std::endl;
		sem_post(&bartender_sem);
	}
	return nullptr;
}

void* bartender(void* arg)
{
	while(true)
	{
		sem_wait(&bartender_sem);
		
		char item;

		std::cin>>item;

		if(item == 't')
		{
			sem_post(&tobacco_sem);
		}
		else if(item == 'p')
		{
			sem_post(&paper_sem);
		}
		else if(item == 'm')
		{
			sem_post(&match_sem);
		}
	}
	return nullptr;
}


int main()
{
	sem_init(&tobacco_sem, 0, 0);
	sem_init(&paper_sem, 0, 0);
	sem_init(&match_sem, 0, 0);
	sem_init(&bartender_sem, 0, 1); 
	   
	pthread_t t_thread, p_thread, m_thread, b_thread;
	           	
	pthread_create(&t_thread, nullptr, smoker_t, nullptr);
	pthread_create(&p_thread, nullptr, smoker_p, nullptr);
	pthread_create(&m_thread, nullptr, smoker_m, nullptr);
	pthread_create(&b_thread, nullptr, bartender, nullptr);
	
	pthread_join(t_thread, nullptr);
	pthread_join(p_thread, nullptr);
	pthread_join(m_thread, nullptr);	
	pthread_join(b_thread, nullptr);
	
	sem_destroy(&tobacco_sem);
	sem_destroy(&paper_sem);
	sem_destroy(&match_sem);
	sem_destroy(&bartender_sem);
	
	return 0;
}
