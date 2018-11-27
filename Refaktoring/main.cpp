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

public:     // ToDo: onTurn + on PassThrough
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
class DiceRoller
{
    uint8_t _numOfDice;
    uint8_t _numOfFaces;

public:
    DiceRoller()
        :_numOfDice(2),
          _numOfFaces(6) {}

    uint8_t rollDice()
    {
        int result = 0;

        for(int i = 0; i < _numOfDice; i++)
        {
            result += std::rand()%_numOfFaces + 1;
        }
        return result;
    }
};



class Board  // ToDo: pętla przechodząca przez pola
{
    std::vector<unique_ptr<Field>> _board;

public:

    Board()
    {
        for(int i{0}; i<40; i++){
            _board.push_back(std::make_unique<Field>());
        }
        _board[0] = std::make_unique<StartField>();
        _board[10] = std::make_unique<PunishField>();
        _board[30] = std::make_unique<RewardField>();
    }

    void setBoard()
    {

    }
    int roll()
    {
        return 6;
    }

    void movePlayer(Player &currentPlayer)
    {
        auto rollValue = roll();
        auto newPos = currentPlayer.getPosition() + rollValue;
        for(auto i{currentPlayer.getPosition() + 1}; i < newPos; i++){
            _board[i%40]->onPass(currentPlayer);
        }
        currentPlayer.setPosition(newPos%40);
        _board[currentPlayer.getPosition()]->onStep(currentPlayer);
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


    Game(std::vector<Player> players):_players(players){}

    bool finished()
    {
        if(turns < 5)
            return false;
        return true;
    }

    void playTurn()
    {
        for(auto pl : _players)
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
    cout << "Hello World!" << endl;

    size_t numPlayers = 2;
    Player player_1("Jan");
    Player player_2("Anna");
    std::vector<Player> players{player_1, player_2};
    Game game(players);

    game.start();

}




