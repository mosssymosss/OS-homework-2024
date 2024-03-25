#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

const int SIZE = 8;

pid_t t_pid, p_pid, m_pid;
int count  = 0;

void smoker_t(sem_t* tobacco_sem, sem_t* bartender_sem)
{
    while(true)
    {
        sem_wait(tobacco_sem);
        std::cout<<'T'<<std::endl;
        sem_post(bartender_sem);
    }
}

void smoker_p(sem_t* paper_sem, sem_t* bartender_sem)
{
    while(true)
    {
        sem_wait(paper_sem);
        std::cout<<'P'<<std::endl;
        sem_post(bartender_sem);
    }
}


void smoker_m(sem_t* match_sem, sem_t* bartender_sem)
{
    while(true)
    {
        sem_wait(match_sem);
        std::cout << 'M' << std::endl;
        sem_post(bartender_sem);
    }
}

void bartender(sem_t* tobacco_sem, sem_t* paper_sem, sem_t* match_sem, sem_t* bartender_sem)
{
    while(true)
    {
        sem_wait(bartender_sem);

        char item;

        if (count == SIZE)
        {
            std::cout << "Out of supplies" << std::endl;
            kill(t_pid, SIGTERM);
            kill(p_pid, SIGTERM);
            kill(m_pid, SIGTERM);
            kill(getpid(), SIGTERM);
        }
        ++count;
        std::cin >> item;

        if (item == 't')
        {
            sem_post(tobacco_sem);
        }
        else if (item == 'p')
        {
            sem_post(paper_sem);
        }
        else if (item == 'm')
        {
            sem_post(match_sem);
        }
        else
        {
            std::cout << "unexpected item" << std::endl;
            kill(t_pid, SIGTERM);
            kill(p_pid, SIGTERM);
            kill(m_pid, SIGTERM);
            kill(getpid(), SIGTERM);
        }
    }
}



int main()
{
    int tobacco_shm = shm_open("/tobacco_mem", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(tobacco_shm == -1)
    {
        std::cerr << "shm_open" << std::endl;
        exit(EXIT_FAILURE);
    }
    int tobacco_trunc = ftruncate(tobacco_shm, sizeof(sem_t));
    if(tobacco_trunc == -1)
    {
        std::cerr << "ftruncate" << std::endl;
        exit(EXIT_FAILURE);
    }


    int paper_shm = shm_open("/paper_mem", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(paper_shm == -1)
    {
        std::cerr << "shm_open" << std::endl;
        exit(EXIT_FAILURE);
    }
    int paper_trunc = ftruncate(paper_shm, sizeof(sem_t));
    if(paper_trunc == -1)
    {
        std::cerr << "ftruncate" << std::endl;
        exit(EXIT_FAILURE);
    }


    int match_shm = shm_open("/match_mem", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(match_shm == -1)
    {
        std::cerr << "shm_open" << std::endl;
        exit(EXIT_FAILURE);
    }
    int match_trunc = ftruncate(match_shm, sizeof(sem_t));
    if(match_trunc == -1)
    {
        std::cerr << "ftruncate" << std::endl;
        exit(EXIT_FAILURE);
    }


    int bartender_shm = shm_open("/bartender_mem", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(bartender_shm == -1)
    {
        std::cerr << "shm_open" << std::endl;
        exit(EXIT_FAILURE);
    }
    int bartender_trunc = ftruncate(bartender_shm, sizeof(sem_t));
    if(bartender_trunc == -1)
    {
        std::cerr << "ftruncate" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t* tobacco_sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, tobacco_shm, 0);
    sem_t* paper_sem= (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, paper_shm, 0);
    sem_t* match_sem= (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, match_shm, 0);
    sem_t* bartender_sem= (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, bartender_shm, 0);

    if(sem_init(tobacco_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(paper_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(match_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(bartender_sem, 1, 1) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }

    t_pid = fork();
    if (t_pid == 0)
    {
        smoker_t(tobacco_sem, bartender_sem);
        return 0;
    }

    p_pid = fork();
    if (p_pid == 0)
    {
        smoker_p(paper_sem, bartender_sem);
        return 0;
    }

    m_pid = fork();
    if (m_pid == 0)
    {
        smoker_m(match_sem, bartender_sem);
        return 0;
    }

    bartender(tobacco_sem, paper_sem, match_sem, bartender_sem);

    waitpid(t_pid, nullptr, 0);
    waitpid(p_pid, nullptr, 0);
    waitpid(m_pid, nullptr, 0);


    if(sem_destroy(bartender_sem) < 0)
    {
        std::cerr << "sem_destroy" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(match_sem) < 0)
    {
        std::cerr << "sem_destroy" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(paper_sem) < 0)
    {
        std::cerr << "sem_destroy" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(tobacco_sem) < 0)
    {
        std::cerr << "sem_destroy" << std::endl;
        exit(EXIT_FAILURE);
    }

    munmap(bartender_sem, sizeof (sem_t));
    munmap(match_sem, sizeof (sem_t));
    munmap(paper_sem, sizeof (sem_t));
    munmap(tobacco_sem, sizeof (sem_t));


    shm_unlink("/bartender_shm");
    shm_unlink("/match_shm");
    shm_unlink("/paper_shm");
    shm_unlink("/tobacco_shm");

    if(close(bartender_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(match_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(paper_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(tobacco_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }

    return 0;
}