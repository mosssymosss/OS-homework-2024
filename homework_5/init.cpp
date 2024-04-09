#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <semaphore.h>

#include "bank.h"

int main()
{
    const int n = 10;
    const char* sem_name = "/sem_shared_mem";
    int sem_shm = shm_open(sem_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(sem_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }
    if(ftruncate(sem_shm, n * sizeof(sem_t)) == -1)
    {
        std::cerr << "sem ftruncate" << std::endl;
        exit(errno);
    }
    sem_t* sems = (sem_t*)mmap(nullptr, n*sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, sem_shm, 0);
//    for(int i = 0; i < n; ++i)
//    {
//        if(sem_init(&sems[i], 1, 1) == -1)
//        {
//            std::cerr << "sem_init" <<std::endl;
//            exit(errno);
//        }
//    }

    const char* client_name = "/client_id_shm";
    int client_shm = shm_open(client_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(client_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }
    if(ftruncate(client_shm, sizeof(int)) == -1)
    {
        std::cerr << "sem ftruncate" << std::endl;
        exit(errno);
    }
    int* id = (int*)mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, client_shm, 0);


    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR , 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }
    //shm open semaphore max 10 hat
    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    if(ftruncate(shm_fd, size) == -1)
    {
        std::cerr << "init ftruncate"<<std::endl;
        exit(errno);
    }

    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    Bank* bank_ptr = (Bank*)ptr;

    bank_ptr->bankSize = n;

    for (int i = 0 ; i < n; ++i) {
        bank_ptr->cells[i] = BankCell();
    }

    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(sems, n * sizeof(sem_t)) == -1)
    {
        std::cerr << "sem init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(id, sizeof(int)) == -1)
    {
        std::cerr << "id init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(sem_shm) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(client_shm) == -1)
    {
        std::cerr << "close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}