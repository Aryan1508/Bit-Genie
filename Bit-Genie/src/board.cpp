#include "board.h"
#include "piece.h"
#include "Square.h"

Rank rank_of(Square sq)
{
	return static_cast<Rank>(sq >> 3);
}

File file_of(Square sq)
{
	return static_cast<File>(sq & 7);
}

Rank rank_of(Square sq, Color color)
{
	return color == Black ? rank_of(flip_square(sq)) : rank_of(sq);
}

std::ostream& operator<<(std::ostream& o, File file)
{
	return o << static_cast<char>((to_int(file) + 97));
}

std::ostream& operator<<(std::ostream& o, Rank file)
{
	return o << static_cast<char>((to_int(file) + 49));
}