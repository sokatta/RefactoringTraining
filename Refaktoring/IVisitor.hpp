#pragma once
#include "namedType.hpp"
#include <string>

class OwnershipAct;
struct IVisitor
{
    virtual void decreaseMoney(Cash val) = 0;
    virtual void increaseMoney(Cash val) = 0;
    virtual bool wantsToBuy(Price price) = 0;
    virtual void assignAct(OwnershipAct* act) = 0;
    virtual std::string name() = 0;
    virtual ~IVisitor() = default;
};
