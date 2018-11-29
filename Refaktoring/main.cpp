#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include "dice.hpp"
#include "namedType.hpp"

using namespace std;

struct OwnershipAct;
class Player;

using Price = NamedType<int, struct PriceParameter>;
using Cash = NamedType<int, struct CashParameter>;
struct IVisitor
{
    virtual void decreaseMoney(int val) = 0;
    virtual void increaseMoney(int val) = 0;
    virtual bool wantsToBuy(Price price) = 0;
    virtual void assignAct(OwnershipAct* act) = 0;
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
    virtual bool buyMansion(Cash cash, Price price) = 0;
};

struct GreedyBuy : DecisionMaker
{
    bool buyMansion(Cash cash, Price price)
    {
        if (cash.get() < price.get())
            return false;
        return true;
    }
};
struct RandomBuy : DecisionMaker
{
    bool buyMansion(Cash cash, Price price)
    {
        if(cash.get() < price.get())
            return false;
        return std::rand()%2;
    }
};

struct HumanBuy : DecisionMaker
{
    bool buyMansion(Cash cash, Price price)
    {
        string decision;
        std::cout << "Masz " << cash.get() << "gotowki, czy chcesz kupic posiadlosc za " << price.get() << "? t/n";
        std::cin >> decision;
        if(decision == "t") 
            return true;
        return false;       
    }
};



class Player : private IVisitor
{
    string _name;
    Cash _cash{500};
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
    void assignAct(OwnershipAct* act) override
    {
        _ownActs.push_back(act);
    }
    Player(string id, FieldIterator iterator, std::unique_ptr<DecisionMaker> buyer)
        :_name(id), _iterator(iterator), buyer(std::move(buyer)) {
        std::cout << "Gracz " << _name << "\n";
    }
    std::string name() override
    {
        return _name;
    }
    bool wantsToBuy(Price price) override
    {
        return buyer->buyMansion(_cash, price);
    }

    void decreaseMoney(int val) override
    {
        _cash.get() -= val;
    }
    void increaseMoney(int val) override
    {
        _cash.get() += val;
    }
    void printName()
    {
        std::cout << _name<<"\n";
    }

    bool isBankrupt(){
        return _cash.get() < 0;
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
    Price price{500};
    uint rent = 300;
public:
        void onStep(IVisitor &player){
            if(!_owner && player.wantsToBuy(price))
            {
               *_owner =  player;
                player.decreaseMoney(price.get());
                player.assignAct(this);
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
                  std::make_shared<Player>("Anna",  board.getIterator(), std::make_unique<RandomBuy>()),
                  std::make_shared<Player>("Ty",  board.getIterator(), std::make_unique<HumanBuy>())})
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




