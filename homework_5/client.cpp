#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <semaphore.h>

#include "bank.h"

int main()
{
    const int n = 10;
    const char* sem_name = "/sem_shared_mem";
    int sem_shm = shm_open(sem_name,  O_RDWR, 0666);
    if(sem_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }

    sem_t* sems = (sem_t*)mmap(nullptr, n*sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, sem_shm, 0);


    const char* client_name = "/client_id_shm";
    int client_shm = shm_open(client_name, O_RDWR, 0666);
    if(client_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }
    int* id = (int*)mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, client_shm, 0);
    (*id)+=1;
    //std::cout<<*id<<std::endl;
    int i = *id - 1;
    if(sem_init(&sems[(*id) - 1], 1, 1) == -1)
    {
        std::cerr << "sem_init" <<std::endl;

        exit(errno);
    }

//    for(int i = 0; i < n; ++i)
//    {
//        if(sem_init(&sems[i], 1, 1) == -1)
//        {
//            std::cerr << "sem_init" <<std::endl;
//            exit(errno);
//        }
//    }
    const char* shm_name = "/bank_shared_mem";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }
    //shm sem open pushback

    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    Bank* ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }
    //std::cout<< ptr->cells[0].max_amount;
    while(true)
    {
        std::string input;
        std::getline(std::cin, input);
        if(input == "exit")
            break;
        std::stringstream str(input);
        std::string temp;
        std::vector<std::string> in;
        while(str >> temp) {
            in.push_back(temp);
        }
        //current 5
        //minimum 5
        //maximum 5
        //freeze 5
        //unfreeze 5
        //transfer 5 6 50
        //addall 50
        //suball 50
        //setmin 5 50
        //setmax 5 50
        if(in.size() == 2)
        {
            if(in[0] == "current")
            {
                sem_wait(&sems[i]);
                int res = ptr->get_cell_curr_balance(std::stoi(in[1]) - 1);
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Current balance for cell number "<<in[1]<<": "<<res<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "minimum")
            {
                sem_wait(&sems[i]);
                int res = ptr->get_cell_min_balance(std::stoi(in[1]) - 1);
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Minimum balance for cell number "<<in[1]<<": "<<res<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "maximum")
            {
                sem_wait(&sems[i]);
                int res = ptr->get_cell_max_balance(std::stoi(in[1]) - 1);
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Maximum balance for cell number "<<in[1]<<": "<<res<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "freeze")
            {
                sem_wait(&sems[i]);
                bool res = ptr->freeze_cell(std::stoi(in[1]) - 1);
                if(!res)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Cell number "<<in[1]<<" successfully frozen"<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "unfreeze")
            {
                sem_wait(&sems[i]);
                bool res = ptr->unfreeze_cell(std::stoi(in[1]) - 1);
                if(!res)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Cell number "<<in[1]<<" successfully unfrozen"<<std::endl;
                sem_post(&sems[*id]);
            }
            else if(in[0] == "addall")
            {
                sem_wait(&sems[i]);
                bool res = ptr->add_to_all(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid operation" <<std::endl;
                else
                    std::cout << "Successfully added " << in[1] <<"to all cells"<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "suball")
            {
                sem_post(&sems[i]);
                bool res = ptr->sub_from_all(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid operation" <<std::endl;
                else
                    std::cout << "Successfully subbed " << in[1] <<"from all cells"<<std::endl;
                sem_post(&sems[i]);
            }
            else
            {
                sem_wait(&sems[i]);
                std::cout<<"invalid input"<<std::endl;
                sem_post(&sems[i]);
            }
        }
        else if(in.size() == 3)
        {
            if(in[0] == "setmin")
            {
                sem_wait(&sems[i]);
                bool res = ptr->set_cell_min_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Minimum for cell "<<in[1]<<" is set to "<<in[2]<<std::endl;
                sem_post(&sems[i]);
            }
            else if(in[0] == "setmax")
            {
                sem_wait(&sems[i]);
                bool res = ptr->set_cell_max_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Maximum for cell "<<in[1]<<" is set to "<<in[2]<<std::endl;
                sem_post(&sems[i]);
            }
            else
            {
                sem_wait(&sems[i]);
                std::cout << "invalid input" <<std::endl;
                sem_post(&sems[i]);
            }
        }
        else if(in.size() == 4)
        {
            if(in[0] == "transfer")
            {
                sem_wait(&sems[i]);
                bool res = ptr->transfer(std::stoi(in[1]) - 1, std::stoi(in[2]) - 1, std::stoi(in[3]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Successfully transferred "<<in[3] <<" from " <<in[1]<<" to "<<in[2]<<std::endl;
                sem_post(&sems[i]);
            }
            else
            {
                sem_wait(&sems[i]);
                std::cout<<"invalid input"<<std::endl;
                sem_post(&sems[i]);
            }
        }
        else
        {
            sem_wait(&sems[i]);
            std::cout<<"invalid input"<<std::endl;
            sem_post(&sems[i]);
        }
    }

//    for (int i = 0; i < n; ++i) {
//        if (sem_destroy(&sems[i]) == -1) {
//            std::cerr << "sem_destroy";
//            exit(EXIT_FAILURE);
//        }
//    }
    if (sem_destroy(&sems[i]) == -1) {
        std::cerr << "sem_destroy";
        exit(EXIT_FAILURE);
    }
    (*id)--;
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