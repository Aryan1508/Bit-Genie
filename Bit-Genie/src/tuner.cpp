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
#include "tuner.h"

#include "eval.h"
#include "position.h"
#include "evalscores.h"

#include <cmath>
#include <vector>
#include <fstream>
#include <string>

#define K (2.97f)

TTuple *TUPLE_STACK;
int TUPLE_STACK_SIZE = STACKSIZE;

double sigmoid(double E) 
{
    return 1.0 / (1.0 + exp(-K * E / 400.0));
}

void init_coeffs(TCoeffs coeffs)
{
    int c = 0;

    for(int i = 0;i < 5;i++) coeffs[c++] = ET.material[i];

    for(int i = 0;i < 6;i++)
        for(int j = 0;j < 64;j++)
            coeffs[c++] = ET.psqt[i][j];

    for(int i = 0;i < 64;i++)
    {
        coeffs[c++] = ET.passed[i];
        coeffs[c++] = ET.passer_blocked[i];
    }

    for(int i = 0;i < 9;i++)   coeffs[c++] = ET.mobility[0][i];
    for(int i = 0;i < 14;i++)  coeffs[c++] = ET.mobility[1][i];
    for(int i = 0;i < 15;i++)  coeffs[c++] = ET.mobility[2][i];
    for(int i = 0;i < 28;i++)  coeffs[c++] = ET.mobility[3][i];
    for(int i = 0;i < 100;i++) coeffs[c++] = ET.safety_table[i];

    coeffs[c++] = ET.isolated;
    coeffs[c++] = ET.stacked;
    coeffs[c++] = ET.passed_connected;
    coeffs[c++] = ET.open_file;
    coeffs[c++] = ET.semi_open_file;
    coeffs[c++] = ET.control;


    if (c != NTERMS)
    {
        std::cerr << "init_coeffs failed, got " << c << " expeceted " << NTERMS << '\n';
        std::terminate();
    }
}

void init_tuner_tuples(TPos *entry, TCoeffs coeffs) {

    static int allocs = 0;
    int length = 0, tidx = 0;

    for (int i = 0; i < NTERMS; i++)
        length += coeffs[i] != 0.0;

    if (length > TUPLE_STACK_SIZE) 
    {
        TUPLE_STACK_SIZE = STACKSIZE;
        TUPLE_STACK = (TTuple*)calloc(STACKSIZE, sizeof(TTuple));
        int ttupleMB = STACKSIZE * sizeof(TTuple) / (1 << 20);
    }

    entry->tuples   = TUPLE_STACK;
    entry->ntuples  = length;
    TUPLE_STACK      += length;
    TUPLE_STACK_SIZE -= length;

    for (int i = 0; i < NTERMS; i++)
        if (coeffs[i] != 0.0)
            entry->tuples[tidx++] = { (int16_t)i, (int16_t)coeffs[i] };
}

void init_tuner_entry(TPos* entry, Position* position)
{
    int phase = get_phase(*position);

    TCoeffs coeffs;
    entry->phase = phase;
    entry->pfactors[0] = phase / 24.0f;
    entry->pfactors[1] = 1 - phase / 24.0f;

    ET.reset();
    entry->seval = position->side == White ? eval_position(*position) : -eval_position(*position);

    init_coeffs(coeffs);
    init_tuner_tuples(entry, coeffs);

    entry->eval = ET.eval;
    entry->turn = position->side;
}

void init_tuner_entries(TPos* entries)
{
    Position position;

    std::ifstream fil(DATASET);

    if (!fil)   
        throw std::runtime_error("Couldn't open tuning file");

    int i = 0;
    for(std::string line; std::getline(fil, line);)
    {
        if (i % (1 << 14) == 0 || i >= NPOSITIONS)
            std::cout << "\rLoading entries... " << i;

        if (i >= NPOSITIONS) break;

        if (line.find("White") != line.npos)      entries[i].result = 1.0;
        else if (line.find("Black") != line.npos) entries[i].result = 0.0;
        else                                      entries[i].result = 0.5;

        std::string fen = line.substr(0, line.find("|") - 1);

        position.set_fen(fen);
        init_tuner_entry(&entries[i++], &position);
    }
    
    std::cout << "\nInitialized " << i << " entries successfully. (" << (sizeof(TPos) * i) / 1e+6 << " Mb)\n";
}

void init_param(double param[2], int value)
{
    param[0] = mg_score(value);
    param[1] = eg_score(value);
}

void init_base_params(TVector params)
{
    int c = 0;

    init_param(params[c++], PawnEval::value);
    init_param(params[c++], KnightEval::value);
    init_param(params[c++], BishopEval::value);
    init_param(params[c++], RookEval::value);
    init_param(params[c++], QueenEval::value);

    for(int i = 0;i < 64;i++)
    {
        init_param(params[c++], PawnEval::psqt[i]);
        init_param(params[c++], PawnEval::passed[i]);
        init_param(params[c++], PawnEval::passer_blocked[i]);
        init_param(params[c++], KnightEval::psqt[i]);
        init_param(params[c++], BishopEval::psqt[i]);
        init_param(params[c++], RookEval::psqt[i]);
        init_param(params[c++], QueenEval::psqt[i]);
        init_param(params[c++], KingEval::psqt[i]);
    }

    for(int i = 0;i < 9;i++)   init_param(params[c++], KnightEval::mobility[i]);
    for(int i = 0;i < 14;i++)  init_param(params[c++], BishopEval::mobility[i]);
    for(int i = 0;i < 15;i++)  init_param(params[c++], RookEval::mobility[i]);
    for(int i = 0;i < 28;i++)  init_param(params[c++], QueenEval::mobility[i]);
    for(int i = 0;i < 100;i++) init_param(params[c++], KingEval::safety_table[i]);

    init_param(params[c++], PawnEval::stacked);
    init_param(params[c++], PawnEval::isolated);
    init_param(params[c++], PawnEval::passed_connected);

    init_param(params[c++], RookEval::open_file);
    init_param(params[c++], RookEval::semi_open_file);

    init_param(params[c++], MiscEval::control);

    if (c != NTERMS)
    {
        std::cerr << "init_base_params failed, got " << c << " expeceted " << NTERMS << '\n';
        std::terminate();
    }
    std::cout << "Initialized " << c << " terms successfully\n";
}

void tune()
{
    TVector current_params = {0};   
    TVector params = {0}, momentum = {0}, velocity = {0};  
    double error, rate = LRRATE;

    TPos* entries = (TPos*)calloc(NPOSITIONS, sizeof(TPos));  
    TUPLE_STACK = (TTuple*)calloc(STACKSIZE, sizeof(TTuple));

    init_tuner_entries(entries);
    init_base_params(current_params);
}