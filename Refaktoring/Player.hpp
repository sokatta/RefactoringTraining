#pragma once
#include <vector>
#include <string>
#include <memory>
#include "IVisitor.hpp"
#include "dice.hpp"
#include "Fields.hpp"
#include "IBrainStrategy.hpp"
#include <iostream>

class Player : private IVisitor
{
    std::string _name;
    Cash _cash{500};
    Dices dices;
    FieldIterator _iterator;
    std::vector<OwnershipAct*> _ownActs;
    std::unique_ptr<IBrainStrategy> buyer;
    uint roundsToWaitInJail{0};

    void passFields(int newPos) {
        for(auto i{0}; i < newPos; i++){
            _iterator.next();
            _iterator.getField().onPass(*this);
        }
    }
    void releaseMansions(){
        for(auto acts : _ownActs){
            acts->releaseOwnership();
        }
    }

    bool notInPrison(){
        return roundsToWaitInJail == 0 ? true:false;
    }

public:
    void sendPlayerToPrison() override{
        roundsToWaitInJail = 3;
    }
    void assignAct(OwnershipAct* act) override
    {
        _ownActs.push_back(act);
    }
    Player(std::string id, FieldIterator iterator, std::unique_ptr<IBrainStrategy> buyer)
            :_name(id), _iterator(iterator), buyer(std::move(buyer)) {
        std::cout << "Gracz " << _name << "\n";
    }
    ~Player(){
        releaseMansions();
    }
    std::string name() override
    {
        return _name;
    }
    bool wantsToBuy(Price price) override
    {
        return buyer->buyMansion(_cash, price);
    }

    void decreaseMoney(Cash val) override
    {
        _cash -= val;
    }
    void increaseMoney(Cash val) override
    {
        _cash += val;
    }
    void printName()
    {
        std::cout << _name<<"\n";
    }

    bool isBankrupt(){
        return _cash.get() < 0;
    }

    void moveAction(){
        if(notInPrison()){
            passFields(dices.roll());
            _iterator.getField().onStep(*this);
        }
        else{
            roundsToWaitInJail--;
        }
    }
};

