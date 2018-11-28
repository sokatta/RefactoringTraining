#pragma once
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
