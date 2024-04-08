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

#include "bank.h"

int main()
{
    const char* shm_name = "/bank_shared_mem";
    const int n = 10;
    int shm_fd = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);

    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    Bank* ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        std::string input;
        std::getline(std::cin, input);
        if(input == "exit")
            break;
        std::stringstream str(input);
        std::string temp;
        std::vector<std::string> in;
        while(str >> temp)
        {
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
                int res = ptr->get_cell_balance(std::stoi(in[1]));
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Current balance for cell number "<<in[1]<<": "<<res<<std::endl;
            }
            else if(in[0] == "minimum")
            {
                int res = ptr->get_cell_min_balance(std::stoi(in[1]));
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Minimum balance for cell number "<<in[1]<<": "<<res<<std::endl;
            }
            else if(in[0] == "maximum")
            {
                int res = ptr->get_cell_max_balance(std::stoi(in[1]));
                if(res == -1)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Maximum balance for cell number "<<in[1]<<": "<<res<<std::endl;
            }
            else if(in[0] == "freeze")
            {
                bool res = ptr->freeze_cell(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Cell number "<<in[1]<<" successfully frozen"<<std::endl;
            }
            else if(in[0] == "unfreeze")
            {
                bool res = ptr->unfreeze_cell(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid id"<<std::endl;
                else
                    std::cout << "Cell number "<<in[1]<<" successfully unfrozen"<<std::endl;
            }
            else if(in[0] == "addall")
            {
                bool res = ptr->add_to_all(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid operation" <<std::endl;
                else
                    std::cout << "Successfully added " << in[1] <<"to all cells"<<std::endl;
            }
            else if(in[0] == "suball")
            {
                bool res = ptr->sub_from_all(std::stoi(in[1]));
                if(!res)
                    std::cout << "invalid operation" <<std::endl;
                else
                    std::cout << "Successfully subbed " << in[1] <<"from all cells"<<std::endl;
            }
            else
            {
                std::cout<<"invalid input"<<std::endl;
            }
        }
        else if(in.size() == 3)
        {
            if(in[0] == "setmin")
            {
                bool res = ptr->set_cell_min_balance(std::stoi(in[1]), std::stoi(in[2]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Minimum for cell "<<in[1]<<" is set to "<<in[2]<<std::endl;
            }
            else if(in[0] == "setmax")
            {
                bool res = ptr->set_cell_max_balance(std::stoi(in[1]), std::stoi(in[2]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Maximum for cell "<<in[1]<<" is set to "<<in[2]<<std::endl;
            }
            else
            {
                std::cout << "invalid input" <<std::endl;
            }
        }
        else if(in.size() == 4)
        {
            if(in[0] == "transfer")
            {
                bool res = ptr->transfer(std::stoi(in[1]), std::stoi(in[2]), std::stoi(in[3]));
                if(!res)
                    std::cout<<"invalid operation"<<std::endl;
                else
                    std::cout<<"Successfully transferred "<<in[3] <<" from " <<in[1]<<" to "<<in[2]<<std::endl;
            }
            else
            {
                std::cout<<"invalid input"<<std::endl;
            }
        }
        else
        {
            std::cout<<"invalid input"<<std::endl;
        }


    }

    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}