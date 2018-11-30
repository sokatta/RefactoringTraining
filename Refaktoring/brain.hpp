#pragma once
#include <string>
#include "IBrainStrategy.hpp"
#include <iostream>
namespace {

    bool operator < (const Cash cash, const Price price){
        if(cash.get() < price.get().get())
            return true;
        return false;
    }

    bool isAfortable(Cash cash, Price price){
        if (cash < price)
            return false;
        return true;
    }
}

struct GreedyBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price)
    {
        return isAfortable(cash, price);
    }
};
struct RandomBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price)
    {
        if(isAfortable(cash, price))
            return false;
        return std::rand()%2;
    }
};

struct HumanBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price) {
        if (isAfortable(cash, price)) {
            std::string decision;
            std::cout << "Masz " << cash << "gotowki, czy chcesz kupic posiadlosc za " << price << "? t/n";
            std::cin >> decision;
            if (decision == "t")
                return true;
            return false;
        }
        return false;
    }
};
