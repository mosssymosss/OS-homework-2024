#include "bank.h"

Bank::Bank(int num)
    :vault(n)
{
}

int Bank::get_cell_balance(int id) const
{
    if(id < 0 || id >= vault.size())
        return -1;
    return vault[id].get_current_balance();
}
int Bank::get_cell_min_balance(int id) const
{
    if(id < 0 || id >= vault.size())
        return -1;
    return vault[id].get_minimum_balance();
}
int Bank::get_cell_max_balance(int id) const
{
    if(id < 0 || id >= vault.size())
        return -1;
    return vault[id].get_maximum_balance();
}

bool Bank::freeze_cell(int id)
{
    if(id < 0 || id >= vault.size())
        return false;
    vault[id].freeze();
    return true;
}
bool Bank::unfreeze_cell(int id)
{
    if(id < 0 || id >= vault.size())
        return false;
    vault[id].unfreeze();
    return true;
}

bool Bank::transfer(int from, int to, int amount)
{
    if(from < 0 || from >= vault.size() || to < 0 || to >= vault.size())
        return false;
    if(amount < 0)
        return false;
    if(vault[from].frozen() || vault[to].frozen())
        return false;

    if(vault[from].send(amount))
    {
        if(vault[to].receive(amount))
        {
            return true;
        }
        vault[from].receive(amount);
    }
    return false;

}

bool Bank::add_to_all(int amount)
{
    if(amount < 0)
        return false;

    int i;
    for(i = 0; i < vault.size(); ++i)
    {
        if(!vault[i].receive(amount) || vault[i].frozen())
        {
            for(int j = 0; j < i; ++j)
            {
                vault[i].receive(amount);
            }
            return false;
        }
    }

    return true;
}
bool Bank::sub_from_all(int amount)
{
    if(amount < 0)
        return false;

    int i;
    for(i = 0; i < vault.size(); ++i)
    {
        if(!vault[i].send(amount) || vault[i].frozen())
        {
            for(int j = 0; j < i; ++j)
            {
                vault[i].send(amount);
            }
            return false;
        }
    }

    return true;
}

bool Bank::set_cell_min_balance(int id, int amount)
{
    if(id < 0 || id >= vault.size())
        return false;
    if(vault[id].frozen())
        return false;

    if(amount > vault[id].get_current_balance() || amount >= vault[id].get_maximum_balance())
        return false;

    vault[id].set_minimum_balance(amount);
    return true;
}
bool Bank::set_cell_max_balance(int id, int amount)
{
    if(id < 0 || id >= vault.size())
        return false;
    if(vault[id].frozen())
        return false;

    if(amount < vault[id].get_current_balance() || amount <= vault[id].get_minimum_balance())
        return false;

    vault[id].set_maximum_balance(amount);
    return true;
}