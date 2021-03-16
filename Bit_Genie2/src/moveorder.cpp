#include "moveorder.h"
#include "search.h"
#include "movelist.h"
#include <algorithm>

static uint64_t least_valuable_attacker(Position& position, uint64_t attackers, Color side, Piece& capturing)
{
	for (int i = 0; i < total_pieces; i++)
	{
		capturing = make_piece(PieceType(i), side);
		uint64_t pieces = position.pieces.bitboards[i] & position.pieces.colors[side] & attackers;

		if (pieces)
			return pieces & ~pieces + 1;
	}
	return 0;
}

static bool lost_material(int16_t scores[16], int index)
{
	return (-scores[index - 1] < 0 && scores[index] < 0);
}

static int16_t see(Position& position, Move move)
{
	static constexpr int see_piece_vals[]{
	  100, 300, 325, 500, 900, 1000,
	  100, 300, 325, 500, 900, 1000, 0
	};

	int16_t scores[32] = { 0 };
	int index = 0;

	Square from = move_from(move);
	Square to = move_to(move);

	Piece capturing = position.pieces.squares[from];
	Piece captured = position.pieces.squares[to];
	Color attacker = color_of(capturing);

	uint64_t from_set = (1ull << from);
	uint64_t occ = position.total_occupancy(), bishops = 0, rooks = 0;

	bishops = rooks = position.pieces.bitboards[Queen];
	bishops |= position.pieces.bitboards[Bishop];
	rooks |= position.pieces.bitboards[Rook];

	uint64_t attack_def = Attacks::attackers_to_sq(position, to);
	scores[index] = see_piece_vals[captured];

	do
	{
		index++;
		attacker = !attacker;
		scores[index] = see_piece_vals[capturing] - scores[index - 1];

		if (lost_material(scores, index))
			break;

		attack_def ^= from_set;
		occ ^= from_set;

		attack_def |= occ & ((Attacks::bishop(to, occ) & bishops) | (Attacks::rook(to, occ) & rooks));
		from_set = least_valuable_attacker(position, attack_def, attacker, capturing);
	} while (from_set);

	while (--index)
	{
		scores[index - 1] = -(-scores[index - 1] > scores[index] ? -scores[index - 1] : scores[index]);
	}

	return scores[0];
}

MovePicker::MovePicker(Position& position, Search& search, TTable&, bool quiescent = false)
{
	switch (stage)
	{
		case MovePicker::Stage::HashMove:
			break;

		case MovePicker::Stage::Killer1:
			break;

		case MovePicker::Stage::Killer2:
			break;

		case MovePicker::Stage::GenNoisy:
			break;

		default:
			break;
	}
}