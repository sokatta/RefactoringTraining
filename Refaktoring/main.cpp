#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include "dice.hpp"
using namespace std;

struct OwnershipAct;
class Player;

class Player;
class Field
{

public:
    virtual void onStep(Player &player){

    }
    virtual void onPass(Player &player)
    {

    }

};

class FieldIterator
{
    uint index;
    std::vector<unique_ptr<Field>>& _board;
    public:
    FieldIterator(std::vector<unique_ptr<Field>>& board)
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

struct IVisitor
{
    virtual void punish(int val) = 0;
    virtual void reward(int val) = 0;
    virtual bool wantsToBuy() = 0;
    virtual string printName() = 0;
};



class Player
{
    string _name;
    int _cash = 500;
    int _position;
    Dices dices;
    FieldIterator _iterator;
    std::vector<OwnershipAct*> _ownActs;

    void passFields(int newPos) {
        for(auto i{0}; i < newPos; i++){
            _iterator.next();
            _iterator.getField().onPass(*this);
        }
    }

public:
    void assignAct(OwnershipAct* act)
    {
        _ownActs.push_back(act);
    }
    Player(string id, FieldIterator iterator)
        :_name(id), _position(0), _iterator(iterator) {
        std::cout << "Gracz " << _name << "\n";
    }
    string name()
    {
        return _name;
    }
    bool wantsToBuy()
    {
        return true;
    }

    void punish(int val)
    {
        _cash -= val;
    }
    void reward(int val)
    {
        _cash += val;
    }
    void printName()
    {
        std::cout << _name<<"\n";
    }

    bool isBankrupt(){
        return _cash < 0;
    }

    void moveAction(){
        passFields(dices.roll());
        _iterator.getField().onStep(*this);
    }

};


class PunishField : public Field
{

public:
    void onStep(Player &player){
        player.punish(100);
    }

};

class RewardField : public Field
{

public:
    void onStep(Player &player){
        player.reward(100);

    }

};

class StartField : public Field{
public:
    void onPass(Player &player){
        player.reward(400);
    }
};

class DepositField : public Field
{
    uint _cash = 0;
    uint _amount = 100;
public:
        void onStep(Player &player){
        player.reward(_cash);
    }
        void onPass(Player &player){
        player.punish(_amount);
        _cash += _amount;
    }
};

struct OwnershipAct
{
    virtual void releaseOwnership()  = 0;
};

class MansionField : public Field, OwnershipAct
{
    std::shared_ptr<Player> _owner = nullptr;
    uint cost = 500;
    uint rent = 300;
public:
        void onStep(Player &player){
            if(!_owner && player.wantsToBuy())
            {
               _owner =  std::make_shared<Player>(player);
               player.punish(cost);
            }
            else if(_owner->name() != player.name())
            {
                player.punish(rent);
                _owner->reward(rent);
            }
        }
        void releaseOwnership() override
        {
            _owner = nullptr;
        }
};


class Board
{
    std::vector<unique_ptr<Field>> _board;
    Dices dices;
    size_t _boardSize;

public:

    Board(size_t boardSize) : _boardSize(boardSize)
    {
        _board.reserve(boardSize);

        for(int i{0}; i<_boardSize; i++){
            _board.push_back(std::make_unique<Field>());
        }
        _board[0] = std::make_unique<StartField>();
        _board[10] = std::make_unique<PunishField>();
        _board[15] = std::make_unique<DepositField>();
        _board[30] = std::make_unique<RewardField>();
    }

    FieldIterator getIterator()
    {
        return FieldIterator(_board);
    }
};

class Game
{
    std::vector<std::shared_ptr<Player>> _players;
    Board board;
    int rounds = 0;


    bool isEnoughPlayers(){
        return _players.size() > 1;
    }
    bool notFinished()
    {
       return isEnoughPlayers() && roundsBelowLimit();
    }
    bool roundsBelowLimit()
    {
        if(rounds < 5)
            return true;
        return false;
    }

public:
    void start()
    {
        while(notFinished())
        {
            playTurn();
            std::cout << "Koniec rundy " << rounds << "\n";
            rounds++;
        }
    }



    Game(size_t boardSie)
        :board(boardSie),
        _players({std::make_shared<Player>("Jan", board.getIterator()), std::make_shared<Player>("Anna",  board.getIterator())})
    {
    }

    void removeBankruptPlayers(){
        _players.erase(std::remove_if(
                _players.begin(),
                _players.end(),
              [](auto _pl){return  _pl->isBankrupt();}),_players.end());
    }

    void playTurn()
    {
        for(auto & pl : _players)
        {
            std::cout << "tura gracza \n";
            pl->printName();
            pl->moveAction();
        }
        removeBankruptPlayers();
    }
};



int main()
{
    Game game(40);
    game.start();
}




