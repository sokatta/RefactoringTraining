#pragma once
#include <vector>
#include <memory>
#include "Fields.hpp"
class Board
{
    std::vector<std::unique_ptr<IField>> _board;
    std::size_t _boardSize;

public:

    Board(size_t boardSize) : _boardSize(boardSize)
    {
        _board.reserve(boardSize);

        for(int i{0}; i<_boardSize; i++){
            _board.push_back(std::make_unique<IField>());
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
