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

public:
    Player(string id)
        :_name(id) {
        std::cout << "Gracz " << _name << "\n";
    }
    //    int roll() const
    //    {
    //        return 6;
    //    }
    void punish(int val)
    {
        _cash -= val;
    }
    void reward(int val)
    {
        _cash += val;
    }
    void call()
    {
        std::cout << _name;
    }

};

class Field
{

public:     // ToDo: onTurn + on PassThrough
    virtual void onStep(Player &player){

    }
    virtual void onPass()
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



//class Board  // ToDo: pętla przechodząca przez pola
//{
//    std::vector<unique_ptr<Field>> _board;

//public:

//    Board():
//        _board(40)
//    {

//    }

//    void setBoard()
//    {
//        _board(40);
//    }

//    void actionOnField(int field)
//    {
//    }
//    void movePlayer(Player& currentPlayer, uint8_t numOfFields )
//    {

//    }

//};

class Game
{

    std::vector<std::unique_ptr<Field>> _board; //ToDo: do osobnej klasy
    std::vector<std::pair<Player, int>> _players;

    int turns = 0;

public:
    void start()
    {
        while(!finished())
            playTurn();
    }


    Game(std::vector<Player> players){
        for(int i{0}; i<40; i++){
            _board.push_back(std::make_unique<Field>());
        }
        _board[10] = std::make_unique<PunishField>();
        for(auto &player : players)
            _players.push_back({player, 0});

    }

    bool finished()
    {
        if(turns < 5)
            return false;
        return true;
    }

    Field current_field(int fild){
        return *_board[fild];
    }

    int roll()
    {
        return 6;
    }
    void playTurn()
    {
        // każdy gracz rzuca kością
        // aktualizacja pozycji
        // wywołanie akji pola
        for(auto pl : _players)
        {
            std::cout << "tura gracza ";
            pl.first.call();
            std::cout << "\n";

            auto rollValue = roll();
            int position = pl.second + rollValue;
            pl.second = (pl.second + rollValue)%40;
            if(position > 40)
            {
                pl.first.reward(100);
            }
            current_field(pl.second).onStep(pl.first);

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

    Game game({player_1, player_2});

    game.start();

//    while(!game.finished()) // ToDo: do impl. Game
//    {
//        game.turn();

//    }
}




