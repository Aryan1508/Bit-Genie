#pragma once
#include <cassert>
#include <chrono>
#include <iostream>
#include <stdint.h>
#include <type_traits>

constexpr int total_pieces = 6;
constexpr int total_colors = 2;
constexpr int total_squares = 64;
constexpr int total_files = 8;
constexpr int total_castle_types = 2;
constexpr int total_ranks = 8;

enum class Direction : int8_t;
enum class File : uint8_t;
enum class MoveGenType : uint8_t;
enum class Rank : uint8_t;
enum class MoveFlag : uint8_t;

enum Move : uint32_t;
enum Square : uint8_t;
enum Piece : uint8_t;
enum PieceType : uint8_t;
enum Color : uint8_t;

class CastleRights;
class Killers;
class Movelist;
class PieceManager;
class Position;
class PositionHistory;
class SHistory;
class TTable;
class ZobristKey;

struct SearchInfo;
struct SearchLimits;
struct Search;
struct TEntry;

template <typename E>
constexpr typename std::underlying_type<E>::type to_int(E e)
{
  return static_cast<typename std::underlying_type<E>::type>(e);
}

inline auto current_time()
{
  using namespace std::chrono;
  return steady_clock::now();
}