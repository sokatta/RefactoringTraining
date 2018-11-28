#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <string>

using namespace std;


class Player
{
    string _name;
    int _cash = 500;
    int _position;

public:
    Player(string id)
        :_name(id), _position(0) {
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

    void setPosition(int val)
    {
        _position = val;
    }

    int getPosition()
    {
        return _position;
    }


};

class Field
{

public:
    virtual void onStep(Player &player){

    }
    virtual void onPass(Player &player)
    {

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
    void onPass(Player &player){
        player.reward(400);
    }
};
class Dices
{
    uint8_t _numOfDice;
    uint8_t _numOfFaces;

public:
    Dices()
        :_numOfDice(2),
          _numOfFaces(6) {}

    uint8_t roll()
    {
        int result = 0;

        for(int i = 0; i < _numOfDice; i++)
        {
            result += std::rand()%_numOfFaces + 1;
        }
        return result;
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
        _board[30] = std::make_unique<RewardField>();
    }

    void boardAction(Player &currentPlayer, int newPos) const {
        for(auto i{currentPlayer.getPosition() + 1}; i < newPos; i++){
            _board[i % _boardSize]->onPass(currentPlayer);
        }
        currentPlayer.setPosition(newPos%_boardSize);
        _board[currentPlayer.getPosition()]->onStep(currentPlayer);
    }

    void movePlayer(Player &currentPlayer)
    {
        auto rollValue = dices.roll();
        auto newPos = currentPlayer.getPosition() + rollValue;
        boardAction(currentPlayer, newPos);
    }

};

class Game
{

    std::vector<Player> _players;
    Board board;
    int turns = 0;

public:
    void start()
    {
        while(!finished())
            playTurn();
    }


    Game(std::vector<Player> players, size_t boardSie):_players(players),board(boardSie){}

    bool finished()
    {
        if(turns < 5)
            return false;
        return true;
    }

    void playTurn()
    {
        for(auto & pl : _players)
        {
            std::cout << "tura gracza \n";
            pl.printName();
            board.movePlayer(pl);
        }
        std::cout << "Koniec rundy " << turns << "\n";
        turns++;
    }
};


int main()
{
    Game game({Player{"Jan"}, Player{"Anna"}}, 40);
    game.start();

}




