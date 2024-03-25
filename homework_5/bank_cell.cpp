#include "bank_cell.h"

BankCell::BankCell()
    :current_balance(0)
    ,minimum_balance(0)
    ,maximum_balance(1000)
    ,is_frozen(false)
{}

int BankCell::get_current_balance() const
{
    return current_balance;
}
int BankCell::get_minimum_balance() const
{
    return minimum_balance;
}
int BankCell::get_maximum_balance() const
{
    return maximum_balance;
}

void BankCell::freeze()
{
    is_frozen = true;
}
void BankCell::unfreeze()
{
    is_frozen = false;
}

bool BankCell::send(int amount)
{
    if(current_balance - amount >= minimum_balance)
    {
        current_balance -= amount;
        return true;
    }
    return false;
}

bool BankCell::receive(int amount)
{
    if(current_balance + amount <= maximum_balance)
    {
        current_balance += amount;
        return true;
    }
    return false;
}

void BankCell::set_minimum_balance(int amount)
{
    minimum_balance = amount;
}
void BankCell::set_maximum_balance(int amount)
{
    maximum_balance = amount;
}

bool BankCell::frozen() const
{
    return is_frozen;
}
