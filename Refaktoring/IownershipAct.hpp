#pragma once
struct OwnershipAct
{
    virtual void releaseOwnership()  = 0;
    virtual ~OwnershipAct() = default;
};