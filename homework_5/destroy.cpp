#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "bank.h"

int main()
{
    const char* sem_name = "/sem_shared_mem";
    int sem_shm = shm_open(sem_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(sem_shm == -1)
    {
        std::cerr << "sem shm open" << std::endl;
        exit(errno);
    }

    const char* client_name = "/client_id_shm";
    int client_shm = shm_open(client_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(client_shm == -1)
    {
        std::cerr << "sem shm open" << std::endl;
        exit(errno);
    }
    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "sem shm open" << std::endl;
        exit(errno);
    }

    if(shm_unlink(sem_name) == -1)
    {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(shm_unlink(client_name) == -1)
    {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(shm_unlink(shm_name) == -1)
    {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(sem_shm) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(client_shm) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }


    return 0;
}