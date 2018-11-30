#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "namedType.hpp"
#include "brain.hpp"
#include "Player.hpp"
#include "Board.hpp"

using namespace std;

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
        _players({std::make_shared<Player>("Jan", board.getIterator(), std::make_unique<GreedyBrain>()),
                  std::make_shared<Player>("Anna",  board.getIterator(), std::make_unique<RandomBrain>()),
                  std::make_shared<Player>("Ty",  board.getIterator(), std::make_unique<HumanBrain>())})
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




