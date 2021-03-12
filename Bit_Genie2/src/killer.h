#pragma once
#include "misc.h"
#include <array>
#include <cstring>

class Killers
{
public:
  Killers()
  {
    std::memset(&moves[0][0], 0, sizeof(moves));
  }

  Move first(int ply)
  {
    if (ply >= 64) {
      std::cout << "Invalid ply\n";
    }
    return moves[ply][0];
  }

  Move second(int ply)
  {
    if (ply >= 64) {
      std::cout << "Invalid ply\n";
    }
    return moves[ply][1];
  }

  void add(int ply, Move move)
  {
    if (ply >= 64) {
      std::cout << "Invalid ply\n";
    }
    moves[ply][1] = moves[ply][0];
    moves[ply][0] = move;
  }
private:
  Move moves[64][2];
};