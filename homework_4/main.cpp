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
        int priorFork = id == 4 ? 0 : id;
        int nonPriorFork = id == 4 ? 4 : id + 1;

		std::cout << id << ": is thinking" << std::endl;
		sleep(rand() % 3);

		if(sem_wait(&sems[priorFork]) < 0)
        {
            std::cerr << "sem_wait" << std::endl;
            exit(EXIT_FAILURE);
        }
		std::cout << id << ": picked up left chopstick" << std::endl;

		if(sem_trywait(&sems[nonPriorFork]) == -1)
		{
			if(sem_post(&sems[priorFork]) < 0)
            {
                std::cerr << "sem_post" << std::endl;
                exit(EXIT_FAILURE);
            }
			std::cout << id << ": released one chopstick" << std::endl;
		}
		else
		{
			std::cout << id << ": picked up right chopstick" << std::endl;
			std::cout << id << ": is eating" << std::endl;
			sleep(rand() % 3);

			if(sem_post(&sems[priorFork]) < 0)
            {
                std::cerr << "sem_post" << std::endl;
                exit(EXIT_FAILURE);
            }
			if(sem_post(&sems[nonPriorFork]) < 0)
            {
                std::cerr << "sem_post" << std::endl;
                exit(EXIT_FAILURE);
            }
			std::cout << id << ": released chopsticks" << std::endl;
		}	

													
	}	
}

int main() 
{
	int shm_fd = shm_open("/shared_mem", O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (shm_fd == -1) 
	{
		std::cerr << "shm_open failed" << std::endl;
		exit(EXIT_FAILURE);
	}
    int trunc = ftruncate(shm_fd, SIZE * sizeof(sem_t));
    if(trunc == -1)
    {
        std::cerr << "ftruncate" << std::endl;
        exit(EXIT_FAILURE);
    }
	sem_t* sems = (sem_t*)mmap(NULL, SIZE * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shm_fd, 0);
	for (int i = 0; i < SIZE; ++i) 
	{
        if(sem_init(&sems[i], 1, 1) < 0)
        {
            std::cerr << "sem_destroy" << std::endl;
            exit(EXIT_FAILURE);
        }
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
		if(sem_destroy(&sems[i]) < 0)
        {
            std::cerr << "sem_destroy" << std::endl;
            exit(EXIT_FAILURE);
        }
	}
						
	munmap(sems, SIZE * sizeof(sem_t));
	shm_unlink("/shared_mem");
    if(close(shm_fd) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    return 0;
}
