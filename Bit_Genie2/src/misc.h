#pragma once
#include <cassert>
#include <iostream>
#include <stdint.h>
#include <type_traits>

// Global constants 
constexpr int total_pieces = 6;
constexpr int total_colors = 2;
constexpr int total_squares = 64;
constexpr int total_files = 8;
constexpr int total_castle_types = 2;

// Forward declarations for most enum(classes)
enum class Direction : int8_t;
enum class File : uint8_t;
enum class MoveGenType : uint8_t;
enum class Rank : uint8_t;
enum class MoveFlag : uint8_t;

enum Square : uint8_t;
enum Piece : uint8_t;
enum PieceType : uint8_t;
enum Color : uint8_t;

// Forward declarations for all classes
class CastleRights;
class Move;
class Movelist;
class PieceManager;
class Position;
class PositionHistory;
class ZobristKey;

// Used for converting enum class types to its underlying type 
template <typename E>
constexpr typename std::underlying_type<E>::type to_int(E e) noexcept
{
  return static_cast<typename std::underlying_type<E>::type>(e);
}

typedef uint64_t uint64_t;