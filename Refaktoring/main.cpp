#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include "dice.hpp"
using namespace std;


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

class Player
{
    string _name;
    int _cash = 500;
    int _position;
    Dices dices;
    FieldIterator _iterator;

    void passFields(int newPos) {
        for(auto i{0}; i < newPos; i++){
            _iterator.next();
            _iterator.getField().onPass(*this);
        }
    }

public:
    Player(string id, FieldIterator iterator)
        :_name(id), _position(0), _iterator(iterator) {
        std::cout << "Gracz " << _name << "\n";
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

class MansionField : public Field
{
    std::shared_ptr<Player> _owner = nullptr;
    uint cost = 500;
    uint rent = 300;
public:
        void onStep(Player &player){
            if(!_owner)
            {
               _owner =  std::make_shared<Player>(player);
               player.punish(cost);
            }
            else
            {
                player.punish(rent);
                _owner->reward(rent);
            }
        }
        void removeOwner()
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
    std::vector<Player> _players;
    Board board;
    int rounds = 0;

public:
    void start()
    {
        while(!finished())
        {
            playTurn();
            std::cout << "Koniec rundy " << rounds << "\n";
            rounds++;
        }
    }
    bool isEnoughPlayers(){
        return _players.size() > 1;
    }
    bool finished()
    {
       return !isEnoughPlayers() || !roundsBelowLimit();
    }
    bool roundsBelowLimit()
    {
        if(rounds < 5)
            return true;
        return false;
    }


    Game(size_t boardSie)
        :board(boardSie),
        _players({Player{"Jan", board.getIterator()}, Player{"Anna",  board.getIterator()}})
    {
    }

    void removeBankrutePlayers(){
//        _players.erase(std::remove_if(
//                _players.begin(),
//                _players.end(),
//              [](auto _pl){return  _pl.isBankrupt();}),_players.end());
    }

    void playTurn()
    {
        for(auto & pl : _players)
        {
            std::cout << "tura gracza \n";
            pl.printName();
            pl.moveAction();
        }
        removeBankrutePlayers();
    }
};



int main()
{
    Game game(40);
    game.start();
}




