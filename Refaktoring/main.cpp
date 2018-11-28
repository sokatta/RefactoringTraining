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
struct IVisitor
{
    virtual void decreaseMoney(int val) = 0;
    virtual void increaseMoney(int val) = 0;
    virtual bool wantsToBuy(int price) = 0;
    virtual std::string name() = 0;
};

class Field
{

public:
    virtual void onStep(IVisitor &player){}
    virtual void onPass(IVisitor &player){}

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

struct DecisionMaker
{
    virtual bool buyMansion(int, int) = 0;
};

struct GreedyBuy : DecisionMaker
{
    bool buyMansion(int cash, int price)
    {
        if (cash < price)
            return false;
        return true;
    }
};
struct RandomBuy : DecisionMaker
{
    bool buyMansion(int cash, int price)
    {
        if(cash < price)
            return false;
        return std::rand()%2;
    }
};



class Player : private IVisitor
{
    string _name;
    int _cash = 500;
    int _position;
    Dices dices;
    FieldIterator _iterator;
    std::vector<OwnershipAct*> _ownActs;
    std::unique_ptr<DecisionMaker> buyer;

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
    Player(string id, FieldIterator iterator, std::unique_ptr<DecisionMaker> buyer)
        :_name(id), _position(0), _iterator(iterator), buyer(std::move(buyer)) {
        std::cout << "Gracz " << _name << "\n";
    }
    std::string name() override
    {
        return _name;
    }
    bool wantsToBuy(int price) override
    {
        return buyer->buyMansion(_cash, price);
    }

    void decreaseMoney(int val) override
    {
        _cash -= val;
    }
    void increaseMoney(int val) override
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
    void onStep(IVisitor &player){
        player.decreaseMoney(100);
    }

};

class RewardField : public Field
{

public:
    void onStep(IVisitor &player){
        player.increaseMoney(100);

    }

};

class StartField : public Field{
public:
    void onPass(IVisitor &player){
        player.increaseMoney(400);
    }
};

class DepositField : public Field
{
    uint _cash = 0;
    uint _amount = 100;
public:
        void onStep(IVisitor &player){
            player.increaseMoney(_cash);
    }
        void onPass(IVisitor &player){
            player.decreaseMoney(_amount);
        _cash += _amount;
    }
};

struct OwnershipAct
{
    virtual void releaseOwnership()  = 0;
};

class MansionField : public Field, OwnershipAct
{
    IVisitor* _owner = nullptr;
    uint cost = 500;
    uint rent = 300;
public:
        void onStep(IVisitor &player){
            if(!_owner && player.wantsToBuy(cost))
            {
               *_owner =  player;
                player.decreaseMoney(cost);
            }
            else if(_owner->name() != player.name())
            {
                player.decreaseMoney(rent);
                _owner->increaseMoney(rent);
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
        _board[35] = std::make_unique<MansionField>();
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
        _players({std::make_shared<Player>("Jan", board.getIterator(), std::make_unique<GreedyBuy>()),
                    std::make_shared<Player>("Anna",  board.getIterator(), std::make_unique<RandomBuy>())})
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




