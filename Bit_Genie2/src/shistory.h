#pragma once
#include "move.h"
#include "position.h"
#include <array>
#include <cstring>

class SHistory
{
public:
  SHistory()
  {
    std::memset(&history[0][0][0], 0, sizeof(history));
  }

  uint16_t& get(Position& position, Move move)
  { 
    return history[position.side][move_from(move)][move_to(move)];
  }

  void add(Position& position, Move move, int depth)
  {
    history[position.side][move_from(move)][move_to(move)] += depth;
  }

private:
  uint16_t history[2][64][64];
};