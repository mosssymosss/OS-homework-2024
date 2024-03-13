#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

const int SIZE = 5;

void philosopher(int id, sem_t* sems) 
{
	while (true) 
	{
		std::cout << id << ": is thinking" << std::endl;
		sleep(rand() % 3);

		sem_wait(&sems[id]);
		std::cout << id << ": picked up left chopstick" << std::endl;

		if(sem_trywait(&sems[(id + 1) % SIZE]) == -1)
		{
			sem_post(&sems[id]);
			std::cout << id << ": released chopstick" << std::endl;
		}
		else
		{
			std::cout << id << ": picked up right chopstick" << std::endl;
			std::cout << id << ": is eating" << std::endl;
			sleep(rand() % 3);

			sem_post(&sems[id]);
			sem_post(&sems[(id + 1) % SIZE]);
			std::cout << id << ": released chopsticks" << std::endl;
		}	

													
	}	
}

int main() 
{
	int shm_fd = shm_open("/shared_mem", O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) 
	{
		std::cerr << "shm_open failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	sem_t* sems = (sem_t*)mmap(NULL, SIZE * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shm_fd, 0);
	for (int i = 0; i < SIZE; ++i) 
	{
		sem_init(&sems[i], 1, 1);
	}

	std::vector<int> pids(5);
	for(int i = 0; i < pids.size(); ++i)
	{
		pids[i] = fork();
		if(pids[i] == 0)
		{
			philosopher(i, sems);
			exit(0);
		}
	}	
	for(int x : pids)
	{
		if(waitpid(x,nullptr,0) == -1)
		{
			std::perror("wait: ");
			exit(EXIT_FAILURE);
		}
	}
					
	for(int i = 0; i < SIZE; ++i) 
	{
		sem_destroy(&sems[i]);
	}
						
	munmap(sems, SIZE * sizeof(sem_t));
	shm_unlink("/shared_mem");
	return 0;
}
