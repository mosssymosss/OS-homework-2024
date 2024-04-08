#ifndef BANK
#define BANK
#include "bank_cell.h"

#include <vector>
class BankCell;
class Bank
{
public:
    Bank(int num);

    int get_cell_balance(int id) const;
    int get_cell_min_balance(int id) const;
    int get_cell_max_balance(int id) const;

    bool freeze_cell(int id);
    bool unfreeze_cell(int id);

    bool transfer(int from, int to, int amount);

    bool add_to_all(int amount);
    bool sub_from_all(int amount);

    bool set_cell_min_balance(int id, int amount);
    bool set_cell_max_balance(int id, int amount);



private:
    std::vector<BankCell> vault;
};

#endif