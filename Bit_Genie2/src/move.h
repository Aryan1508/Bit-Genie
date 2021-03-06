#pragma once
#include "misc.h"

enum class MoveFlag : uint8_t {
  normal, enpassant, castle, promotion
};

#define Move(from, to, type, promoted) (((to_int(from))) | ((to_int(to)) << 6) | ((to_int(type)) << 12) | (((to_int(promoted) - 1) << 14)))
#define GetMoveFrom(move) Square((move) & 0x3f)
#define GetMoveTo(move) Square(((move) >> 6) & 0x3f)
#define GetMoveType(move) MoveFlag(((move) >> 12) & 0x3)
#define GetMovePromoted(move) PieceType((((move) >> 14) & 0b11) + 1)

bool move_is_capture(Position const& positio, uint16_t);
std::string print_move(uint16_t);