#pragma once
#include "misc.h"
#include "movelist.h"
#include "bitboard.h"
#include "position.h"

enum class MoveGenType { normal, noisy, quiet };

template<MoveGenType type>
class MoveGenerator
{
public:


private:
  Bitboard get_targets(Position const& position) const
  {
    
  }


private:
  Movelist movelist;
};