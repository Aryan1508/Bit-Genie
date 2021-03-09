#pragma once
#include "move.h"
#include <array>
#include "position.h"

class SHistory
{
public:
  uint16_t& get(Position& position, Move move)
  {
    return history[move & 0xFFFF];
  }

  void add(Position& position, Move move, int depth)
  {
    get(position, move) = depth * depth;
  }

private:
  uint16_t history[0xFFFF] = { 0 };
};