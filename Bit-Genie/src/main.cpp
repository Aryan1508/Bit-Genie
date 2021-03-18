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

int main()
{
	Attacks::init();
	ZobristKey::init();
    uci_input_loop();
}