#pragma once
#include "Field.hpp"
#include "IVisitor.hpp"
#include "IownershipAct.hpp"
#include <memory>
#include <vector>
struct IVisitor;



class RandomField : public Field
{
    std::vector<std::unique_ptr<Field>> fieldPool;
    int _poolSize;
    int generateRandomIndex() 
    {
        return std::rand()%_poolSize;
    }

public:
    RandomField(std::initializer_list<Field> fields)
    {
        for(auto el : fields)
            fieldPool.push_back(std::make_unique<Field>(el));
        _poolSize = fieldPool.size();
    }
    RandomField() : _poolSize(1)
    {
        for(auto i = 0; i < _poolSize; i++)
            fieldPool.push_back(std::make_unique<Field>());
    }
    void onStep(IVisitor &player) override
    {
        int fieldIndex = generateRandomIndex();
        fieldPool[fieldIndex]->onStep(player);
    }
    void onPass(IVisitor &player) override
    {
        int fieldIndex = generateRandomIndex();
        fieldPool[fieldIndex]->onPass(player);
    }
};



class PunishField : public Field
{

public:
    void onStep(IVisitor &player){
        player.decreaseMoney(Cash(100));
    }

};

class RewardField : public Field
{

public:
    void onStep(IVisitor &player){
        player.increaseMoney(Cash{100});

    }

};

class StartField : public Field{
public:
    void onPass(IVisitor &player){
        player.increaseMoney(Cash{400});
    }
};

class DepositField : public Field
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

class MansionField : public Field, private OwnershipAct
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

class PrisonField : public Field{
public:
    void onStep(IVisitor &player) override{
        player.sendPlayerToPrison();
    }
};

class BlackHoleField : public Field{
public:
    BlackHoleField(std::unique_ptr<Field> underField): underField(std::move(underField)) {}
    void onStep(IVisitor &player) override{
        isActive = flipBehaviur();
        if(isActive)
            underField->onStep(player);
    }
    void onPass(IVisitor &player) override{
        isActive = flipBehaviur();
        if(isActive)
            underField->onPass(player);
    }
private:
    bool flipBehaviur(){
        return isActive ? false:true;
    }
    std::unique_ptr<Field> underField;
    bool isActive{false};
};

class FieldIterator
{
    uint index;
    std::vector<std::unique_ptr<Field>>& _board;
public:
    FieldIterator(std::vector<std::unique_ptr<Field>>& board)
    : _board(board), index(0){}

    void next()
    {
        index = (index + 1)%_board.size();
    }
    Field& getField()
    {
        return *_board[index];
    }
};
