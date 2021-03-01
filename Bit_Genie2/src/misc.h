#pragma once
#include <stdint.h>
#include <type_traits>
#include <cassert>

constexpr int total_pieces = 6;
constexpr int total_colors = 2;
constexpr int total_squares = 64;
constexpr int total_files = 8;
constexpr int total_castle_types = 2;

enum class File : uint8_t;
enum class Rank : uint8_t;
enum class Direction : int8_t;
enum class Square : uint8_t;
class Bitboard;
class Piece;
class ZobristKey;
class Position;
class PositionHistory;

// Used for converting enum class types to its underlying type 
// Example: Square -> uint8_t
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
  return static_cast<typename std::underlying_type<E>::type>(e);
}