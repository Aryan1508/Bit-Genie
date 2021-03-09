#pragma once
#include "misc.h"
#include <array>

class Killers
{
public:
  Killers() = default;

  uint16_t first(int ply)
  {
    return moves[ply][0];
  }

  uint16_t second(int ply)
  {
    return moves[ply][1];
  }

  void add(int ply, uint16_t move)
  {
    moves[ply][1] = moves[ply][0];
    moves[ply][0] = move;
  }
private:
  uint16_t moves[64][2] = { 0 };
};