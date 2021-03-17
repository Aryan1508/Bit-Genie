#include "bitboard.h"
#include "castle_rights.h"
#include "io.h"
#include "piece.h"
#include "position.h"
#include "Square.h"
#include "move.h"
#include "movegen.h"
#include "uci.h"
#include "moveorder.h"
#include "zobrist.h"
#include <queue>

void check_pseudolegal(Position& position, int depth)
{
	if (depth <= 0)
		return;

	MoveGenerator gen;
	gen.generate(position);

	for (auto m : gen.movelist)
	{
		if (!position.move_is_pseudolegal(m))
		{
			std::cout << position;
			std::cout << "Error for " << print_move(m) << std::endl;
			std::cin.get();
		}
		position.apply_move(m);
		check_pseudolegal(position, depth - 1);
		position.revert_move();
	}
}

int main()
{
	Attacks::init();
	ZobristKey::init();
    uci_input_loop();
	Position position;
	position.set_fen("rnb1kbnr/ppp1pppp/8/3Q4/4p3/8/PPPP1qPP/RNBK1BNR w kq - 0 1");

	MoveGenerator<false> gen;
	gen.generate(position);

	for (auto m : gen.movelist)
		std::cout << print_move(m) << std::endl;

	Move move = CreateMove(D5, E6, MoveFlag::normal, 1);
	
	std::cout << "Move is pseudo-legal: " << position.move_is_pseudolegal(move) << std::endl;
}