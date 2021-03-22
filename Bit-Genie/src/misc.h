/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <chrono>
#include <iostream>
#include <stdint.h>
#include <type_traits>
#include <cassert>


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

inline int64_t current_time()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}