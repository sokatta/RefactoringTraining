#pragma once
#include <vector>
#include <string>
#include <memory>
#include "IVisitor.hpp"
#include "dice.hpp"
#include "Fields.hpp"
#include "IBrainStrategy.hpp"
#include <iostream>

class IPlayerState{
public:
    virtual std::unique_ptr<IPlayerState> moveAction() = 0;
};

class IPlayer {
public:
    virtual void moveMe() = 0;
};

class IPin {
public:
    virtual void movePin() = 0;
    virtual void printName() = 0;
    virtual bool isBankrupt() = 0;
    virtual ~IPin() = default;
};


class ActivePlayerState : public IPlayerState{
public:
    ActivePlayerState(IPlayer &player) : player(player) {}
    std::unique_ptr<IPlayerState> moveAction() override {
        player.moveMe();
        return std::make_unique<ActivePlayerState>(player);
    }
private:
    IPlayer &player;
};

class PlayerInPrison : public IPlayerState{
public:
    PlayerInPrison(IPlayer &player) : player(player) {}
    PlayerInPrison(IPlayer &player, uint roundsToWaitInJail) : player(player), roundsToWaitInJail(roundsToWaitInJail) {}
private:
    std::unique_ptr<IPlayerState> moveAction() override{
        roundsToWaitInJail--;
        if(roundsToWaitInJail == 0)
            return std::make_unique<ActivePlayerState>(player);
        else
            return std::make_unique<PlayerInPrison>(player, roundsToWaitInJail);
    }
private:
    IPlayer &player;
    uint roundsToWaitInJail{3};
};

class Player : private IVisitor, public IPlayer, public IPin
{
    std::string _name;
    Cash _cash{1000};
    Dices dices;
    FieldIterator _iterator;
    std::vector<OwnershipAct*> _ownActs;
    std::unique_ptr<IBrainStrategy> buyer;
    std::unique_ptr<IPlayerState> playerState;

    void passFields(int newPos) {
        for(auto i{0}; i < newPos; i++){
            _iterator.next();
            _iterator.getField().onPass(*this);
        }
    }
    virtual void releaseMansions(){
        for(auto acts : _ownActs){
            acts->releaseOwnership();
        }
    }

public:

    Player(std::string id, FieldIterator iterator, std::unique_ptr<IBrainStrategy> buyer)
            :_name(id), _iterator(iterator), buyer(std::move(buyer)), playerState(std::make_unique<ActivePlayerState>(*this)) {
        std::cout << "Gracz " << _name << "\n";
    }

    void moveMe() override{
        passFields(dices.roll());
        _iterator.getField().onStep(*this);
    }
    void sendPlayerToPrison() override{
        playerState = std::make_unique<PlayerInPrison>(*this);
    }
    void assignAct(OwnershipAct* act) override
    {
        _ownActs.push_back(act);
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
    void printName() override
    {
        std::cout << _name<<"\n";
    }

    bool isBankrupt() override{
        if (_cash.get() < 0){
            releaseMansions();
            return true;
        }
        return false;
    }

    void movePin() override {
        playerState = playerState->moveAction();
    }

};

