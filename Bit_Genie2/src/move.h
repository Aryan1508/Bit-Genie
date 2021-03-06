#pragma once
#include "misc.h"

enum class MoveFlag : uint8_t
{
  normal, enpassant, castle, promotion
};

#define Move(from, to, type, promoted) (((from)) | ((to) << 6) | ((to_int(type)) << 12) | (((to_int(promoted) - 1) << 14)))

constexpr Square move_from(uint16_t move)
{
  return static_cast<Square>(move & 0x3f);
}

constexpr Square move_to(uint16_t move)
{
  return static_cast<Square>((move >> 6) & 0x3f);
}

constexpr MoveFlag move_flag(uint16_t move)
{
  return static_cast<MoveFlag>((move >> 12) & 0x3);
}

constexpr PieceType move_promoted(uint16_t move)
{
  return static_cast<PieceType>(((move >> 14) & 0xb11) + 1);
}

bool move_is_capture(Position const& positio, uint16_t);
std::string print_move(uint16_t);