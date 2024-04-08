#ifndef BANK_CELL
#define BANK_CELL
#include "bank.h"

class BankCell
{
    friend class Bank;
public:

    BankCell();
    int get_current_balance() const;
    int get_minimum_balance() const;
    int get_maximum_balance() const;

    void freeze();
    void unfreeze();

    bool send(int amount);
    bool receive(int amount);

    void set_minimum_balance(int amount);
    void set_maximum_balance(int amount);

    bool frozen() const;

private:
    int current_balance;
    int minimum_balance;
    int maximum_balance;
    bool is_frozen;
};

#endif