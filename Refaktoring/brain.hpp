#pragma once
#include <string>
#include "IBrainStrategy.hpp"
#include <iostream>

struct GreedyBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price)
    {
        if (cash <= price)
            return false;
        return true;
    }
};
struct RandomBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price)
    {
        if(cash <= price)
            return false;
        return std::rand()%2;
    }
};

struct HumanBrain : IBrainStrategy
{
    bool buyMansion(Cash cash, Price price)
    {
        std::string decision;
        std::cout << "Masz " << cash << "gotowki, czy chcesz kupic posiadlosc za " << price << "? t/n";
        std::cin >> decision;
        if(decision == "t")
            return true;
        return false;
    }
};
