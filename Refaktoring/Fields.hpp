#pragma once
#include "IField.hpp"
#include "IVisitor.hpp"
#include "IownershipAct.hpp"
#include <memory>
#include <vector>
struct IVisitor;

class PunishField : public IField
{

public:
    void onStep(IVisitor &player){
        player.decreaseMoney(Cash(100));
    }

};

class RewardField : public IField
{

public:
    void onStep(IVisitor &player){
        player.increaseMoney(Cash{100});

    }

};

class StartField : public IField{
public:
    void onPass(IVisitor &player){
        player.increaseMoney(Cash{400});
    }
};

class DepositField : public IField
{
    Cash storedCash{0};
    const Cash amount{100};
public:
    void onStep(IVisitor &player){
        player.increaseMoney(storedCash);
        storedCash = Cash(0);
    }
    void onPass(IVisitor &player){
        player.decreaseMoney(amount);
        storedCash += amount;
    }
};

class MansionField : public IField, private OwnershipAct
{
    IVisitor* _owner = nullptr;
    Price price{Cash{500}};
    uint rent = 300;

    bool fieldAvailableToBuy(){ return _owner == nullptr; }
    bool visitingPlayerIsOwner(IVisitor &player)
    {
        if(fieldAvailableToBuy())
            return false;
        return _owner->name() == player.name();
    }

    void getRentFrom(IVisitor &player)
    {
        player.decreaseMoney(Cash{rent});
        _owner->increaseMoney(Cash{rent});
    }
    void makeTransactionBy(IVisitor &player)
    {
        if (player.wantsToBuy(price))
        {
            *_owner =  player;
            player.decreaseMoney(price.get());
            player.assignAct(this);
        }
    }
public:
    void onStep(IVisitor &player){
        if(fieldAvailableToBuy())
        {
            makeTransactionBy(player);
        }
        else if(not visitingPlayerIsOwner(player))
        {
           getRentFrom(player);
        }
    }
    void releaseOwnership() override
    {
        _owner = nullptr;
    }
};

class FieldIterator
{
    uint index;
    std::vector<std::unique_ptr<IField>>& _board;
public:
    FieldIterator(std::vector<std::unique_ptr<IField>>& board)
    : _board(board), index(0){}

    void next()
    {
        index = (index + 1)%_board.size();
    }
    IField& getField()
    {
        return *_board[index];
    }
};
