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
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

#define K (2.97f)
#define MG (0)
#define EG (1)

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

    int length = 0, tidx = 0;

    for (int i = 0; i < NTERMS; i++)
        length += coeffs[i] != 0.0;

    if (length > TUPLE_STACK_SIZE) 
    {
        TUPLE_STACK_SIZE = STACKSIZE;
        TUPLE_STACK = (TTuple*)calloc(STACKSIZE, sizeof(TTuple));
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
    uint64_t knights = position->pieces.bitboards[Knight];
    uint64_t bishops = position->pieces.bitboards[Bishop];
    uint64_t rooks   = position->pieces.bitboards[Rook];
    uint64_t queens  = position->pieces.bitboards[Queen];

    int phase = 24 - 4 * popcount64(queens)
                   - 2 * popcount64(rooks) 
                   - 1 * popcount64(bishops | knights);

    entry->pfactors[MG] = 0 + phase / 24.0f;
    entry->pfactors[EG] = 1 - phase / 24.0f;
    entry->phase        = (phase * 256 + 12) / 24.0f;

    ET.reset();
    entry->seval = position->side == White ? eval_position(*position) : -eval_position(*position);

    TCoeffs coeffs;
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
    fil.close();
}

void init_param(double param[2], int value)
{
    param[0] = mg_score(value);
    param[1] = eg_score(value);
}

void init_base_params(TVector params)
{
    int c = 0;

    // Pawn eval
    init_param(params[c++], PawnEval::value);
    init_param(params[c++], PawnEval::stacked);
    init_param(params[c++], PawnEval::isolated);
    init_param(params[c++], PawnEval::passed_connected);
    for(int i = 0;i < 64;i++) init_param(params[c++], PawnEval::psqt[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], PawnEval::passed[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], PawnEval::passer_blocked[i]);

    // Knight eval 
    init_param(params[c++], KnightEval::value);
    for(int i = 0;i < 9;i++)  init_param(params[c++], KnightEval::mobility[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], KnightEval::psqt[i]);

    // Bishop eval
    init_param(params[c++], BishopEval::value);
    for(int i = 0;i < 14;i++)  init_param(params[c++], BishopEval::mobility[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], BishopEval::psqt[i]);

    // Rook eval 
    init_param(params[c++], RookEval::value);
    init_param(params[c++], RookEval::open_file);
    init_param(params[c++], RookEval::semi_open_file);
    for(int i = 0;i < 15;i++)  init_param(params[c++], RookEval::mobility[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], RookEval::psqt[i]);

    // Queen eval 
    init_param(params[c++], QueenEval::value);
    for(int i = 0;i < 28;i++)  init_param(params[c++], QueenEval::mobility[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], QueenEval::psqt[i]);

    // King eval
    for(int i = 0;i < 64;i++) init_param(params[c++], KingEval::psqt[i]);
    for(int i = 0;i < 100;i++) init_param(params[c++], KingEval::safety_table[i]);

    // Misc eval
    init_param(params[c++], MiscEval::control);

    if (c != NTERMS)
    {
        std::cerr << "init_base_params failed, got " << c << " expeceted " << NTERMS << '\n';
        std::terminate();
    }
    std::cout << "Initialized " << c << " terms successfully\n";
}

double linear_evaluation(TPos *entry, TVector params)
 {

    double midgame = mg_score(entry->eval);
    double endgame = eg_score(entry->eval);

    for (int i = 0; i < entry->ntuples; i++) {
        midgame += (double) entry->tuples[i].coeff * params[entry->tuples[i].index][MG];
        endgame += (double) entry->tuples[i].coeff * params[entry->tuples[i].index][EG];
    }

    double mixed =  (midgame * entry->phase
                  +  endgame * (256 - entry->phase))
                  / 256;

    return mixed;
}

void update_single_gradient(TPos *entry, TVector gradient, TVector params) 
{

    double E = linear_evaluation(entry, params);
    double S = sigmoid(E);
    double X = (entry->result - S) * S * (1 - S);

    double mgBase = X * entry->pfactors[MG];
    double egBase = X * entry->pfactors[EG];

    for (int i = 0; i < entry->ntuples; i++) {
        int index = entry->tuples[i].index;
        int coeff = entry->tuples[i].coeff;

        gradient[index][MG] += mgBase * coeff;
        gradient[index][EG] += egBase * coeff;
    }
}

void compute_gradient(TPos *entries, TVector gradient, TVector params)
{
    #pragma omp parallel shared(gradient)
    {
        TVector local = {0};
        #pragma omp for schedule(static, NPOSITIONS / NPARTITIONS)
        for (int i = 0; i < NPOSITIONS; i++)
            update_single_gradient(&entries[i], local, params);

        for (int i = 0; i < NTERMS; i++) {
            gradient[i][MG] += local[i][MG];
            gradient[i][EG] += local[i][EG];
        }
    }
}

double tune_evaluation_error(TPos *entries, TVector params) 
{
    double total = 0.0;

    #pragma omp parallel shared(total)
    {
        #pragma omp for schedule(static, NPOSITIONS / NPARTITIONS) reduction(+:total)
        for (int i = 0; i < NPOSITIONS; i++)
            total += pow(entries[i].result - sigmoid(linear_evaluation(&entries[i], params)), 2);
    }

    return total / (double) NPOSITIONS;
}

std::string print_param(double param[2])
{
    std::stringstream o;
    o << "S(" << std::setw(4) << (int)round(param[MG]) << ", " << std::setw(4) << (int)round(param[EG]) << ")";
    return o.str();
}

void print_single(double param[2], std::string_view name, std::ostream& o)
{
    o << "\n    constexpr int " << name << " = " << print_param(param) << ";\n";
}

void print_array(TVector params, int count, std::string_view name, std::ostream& o, int& offset)
{
    o << "\n    constexpr int " << name << "[" << count << "]\n    {";

    for(int i = 0;i < count;i++)
    {
        if (i % 8 == 0)
            o << "\n        ";
        
        o << print_param(params[offset + i]) << ", ";
    }
    o << "\n    };\n";
    offset += count;
}

void save_params(TVector params, TVector current_params)    
{
    TVector tparams;

    for(int i = 0;i < NTERMS;i++)
    {
        tparams[i][MG] = round(params[i][MG] + current_params[i][MG]);
        tparams[i][EG] = round(params[i][EG] + current_params[i][EG]);
    }

    std::ofstream fil("tuned.txt");
    int c = 0;

    fil << "\nnamespace PawnEval\n{";
    print_single(tparams[c++], "value", fil);
    print_single(tparams[c++], "stacked", fil);
    print_single(tparams[c++], "isolated", fil);
    print_single(tparams[c++], "passed_connected", fil);
    print_array(tparams, 64, "psqt", fil, c);
    print_array(tparams, 64, "passed", fil, c);
    print_array(tparams, 64, "passer_blocked", fil, c);
    fil << "}\n";

    fil << "\nnamespace KnightEval\n{";
    print_single(tparams[c++], "value", fil);
    print_array(tparams, 9, "mobility", fil, c);
    print_array(tparams, 64, "psqt", fil, c);
    fil << "}\n";

    fil << "\nnamespace BishopEval\n{";
    print_single(tparams[c++], "value", fil);
    print_array(tparams, 14, "mobility", fil, c);
    print_array(tparams, 64, "psqt", fil, c);
    fil << "}\n";

    fil << "\nnamespace RookEval\n{";
    print_single(tparams[c++], "value", fil);
    print_single(tparams[c++], "open_file", fil);
    print_single(tparams[c++], "semi_open_file", fil);
    print_array(tparams, 15, "mobility", fil, c);
    print_array(tparams, 64, "psqt", fil, c);
    fil << "}\n";

    fil << "\nnamespace QueenEval\n{";
    print_single(tparams[c++], "value", fil);
    print_array(tparams, 28, "mobility", fil, c);
    print_array(tparams, 64, "psqt", fil, c);
    fil << "}\n";

    fil << "\nnamespace KingEval\n{";
    print_array(tparams, 64, "psqt", fil, c);
    print_array(tparams, 100, "safety_table", fil, c);
    fil << "}\n";

    fil << "\nnamespace MiscEval\n{";
    print_single(tparams[c++], "control", fil);
    fil << "}\n";

    if (c != NTERMS)
    {
        std::cerr << "Error in printing the params. Got " << c << " expceted " << NTERMS << '\n';
        std::terminate();
    }

    fil.close();
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
    save_params(current_params, params);

    for (int epoch = 1; epoch <= 1000; epoch++) {

        TVector gradient = {0};
        compute_gradient(entries, gradient, params);

        for (int i = 0; i < NTERMS; i++) 
        {
            double mg_grad = (-K / 200.0) * gradient[i][MG] / NPOSITIONS;
            double eg_grad = (-K / 200.0) * gradient[i][EG] / NPOSITIONS;

            momentum[i][MG] = ADAM_BETA1 * momentum[i][MG] + (1.0 - ADAM_BETA1) * mg_grad;
            momentum[i][EG] = ADAM_BETA1 * momentum[i][EG] + (1.0 - ADAM_BETA1) * eg_grad;

            velocity[i][MG] = ADAM_BETA2 * velocity[i][MG] + (1.0 - ADAM_BETA2) * pow(mg_grad, 2);
            velocity[i][EG] = ADAM_BETA2 * velocity[i][EG] + (1.0 - ADAM_BETA2) * pow(eg_grad, 2);

            params[i][MG] -= rate * momentum[i][MG] / (1e-8 + sqrt(velocity[i][MG]));
            params[i][EG] -= rate * momentum[i][EG] / (1e-8 + sqrt(velocity[i][EG]));
        }

        error = tune_evaluation_error(entries, params);
        printf("\rEpoch [%d] Error = [%.8f], Rate = [%g]", epoch, error, rate);

        if (epoch % 64 == 0) save_params(params, current_params);    
    }
}