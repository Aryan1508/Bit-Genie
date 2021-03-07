#include "bitboard.h"
#include "castle_rights.h"
#include "io.h"
#include "piece.h"
#include "position.h"
#include "Square.h"
#include "move.h"
#include "movegen.h"
#include "uci.h"
#include "zobrist.h"

int main()
{  
  Attacks::init(); 
  ZobristKey::init();
  uci_input_loop();
}