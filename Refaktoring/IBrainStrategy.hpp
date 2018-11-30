#pragma once
#include "namedType.hpp"
struct IBrainStrategy
{
    virtual bool buyMansion(Cash cash, Price price) = 0;
};
