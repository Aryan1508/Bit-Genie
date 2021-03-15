#pragma once
#include "movegen.h"
#include "tt.h"
#include "killer.h"
#include "position.h"

enum class MoveOrderStage
{

};

void sort_movelist(Movelist&, Position&, Search&, TTable&);
void sort_qmovelist(Movelist&, Position&, Search&, TTable&);
int16_t see(Position&, Move);