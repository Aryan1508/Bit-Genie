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

/*
    Tuner implementation based on "Evaluation & Tuning in Chess Engines" by Andrew Grant
    https://github.com/AndyGrant/Ethereal/blob/master/Tuning.pdf
*/

#ifdef TUNE
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

#define K  (2.97f)
#define MG (0)
#define EG (1)

double sigmoid(double E) 
{
    return 1.0 / (1.0 + exp(-K * E / 400.0));
}

void init_coeffs(TCoeffs coeffs)
{
    int c = 0;

    coeffs[c++] = ET.material[Pawn];
    coeffs[c++] = ET.stacked;
    coeffs[c++] = ET.isolated;
    coeffs[c++] = ET.supported_passer;
    coeffs[c++] = ET.passer_tempo;
    coeffs[c++] = ET.support;
    for(int i = 0;i <  8;i++) coeffs[c++] = ET.phalanx[i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[Pawn][i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.passer[i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.blocked_passer[i];

    coeffs[c++] = ET.material[Knight];
    for(int i = 0;i < 9;i++) coeffs[c++] =  ET.mobility[Knight][i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[Knight][i];

    coeffs[c++] = ET.material[Bishop];
    for(int i = 0;i < 14;i++) coeffs[c++] = ET.mobility[Bishop][i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[Bishop][i];

    coeffs[c++] = ET.material[Rook];
    coeffs[c++] = ET.open_file;
    coeffs[c++] = ET.semi_open_file;
    coeffs[c++] = ET.friendly_file;
    coeffs[c++] = ET.friendly_rank;
    for(int i = 0;i < 15;i++) coeffs[c++] = ET.mobility[Rook][i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[Rook][i];

    coeffs[c++] = ET.material[Queen];
    for(int i = 0;i < 28;i++) coeffs[c++] = ET.mobility[Queen][i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[Queen][i];

    for(int i = 0;i <  4;i++) coeffs[c++] = ET.pawn_shield[i];
    for(int i = 0;i < 64;i++) coeffs[c++] = ET.psqt[King][i];
    for(int i = 0;i < 100;i++) coeffs[c++] = ET.safety_table[i];

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

    entry->tuples   = (TTuple*)malloc(sizeof(TTuple) * length);
    entry->ntuples  = length;

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

    entry->pfactors[MG] = 1 - phase / 24.0f;
    entry->pfactors[EG] = 0 + phase / 24.0f;
    entry->phase        = (phase * 256 + 12) / 24.0f;

    ET.reset();
    entry->seval = position->side == White ? Eval::evaluate(*position) : -Eval::evaluate(*position);

    TCoeffs coeffs;
    init_coeffs(coeffs);
    init_tuner_tuples(entry, coeffs);

    entry->eval = ET.eval;
    entry->turn = position->side;
    entry->scale = Eval::get_scale_factor(*position, entry->seval) / 128.0f;
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

        if (line.find("[1.0]") != line.npos)      entries[i].result = 1.0;
        else if (line.find("[0.0]") != line.npos) entries[i].result = 0.0;
        else                                      entries[i].result = 0.5;

        std::string fen = line.substr(0, line.find("[") - 1);

        position.set_fen(fen);
        init_tuner_entry(&entries[i++], &position);
    }
    
    std::cout << "\nInitialized " << i << " entries successfully\n";
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
    init_param(params[c++], PAWN_VALUE);
    init_param(params[c++], PAWN_STACKED);
    init_param(params[c++], PAWN_ISOLATED);
    init_param(params[c++], SUPPORTED_PASSER);
    init_param(params[c++], PASSER_TEMPO);
    init_param(params[c++], PAWN_SUPPORT);
    for(int i = 0;i <  8;i++) init_param(params[c++], PHALANX[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], PAWN_PSQT[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], PASSER[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], BLOCKED_PASSER[i]);

    // Knight eval 
    init_param(params[c++], KNIGHT_VALUE);
    for(int i = 0;i < 9;i++)  init_param(params[c++], KNIGHT_MOBILITY[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], KNIGHT_PSQT[i]);

    // Bishop eval
    init_param(params[c++], BISHOP_VALUE);
    for(int i = 0;i < 14;i++)  init_param(params[c++], BISHOP_MOBILITY[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], BISHOP_PSQT[i]);

    // Rook eval 
    init_param(params[c++], ROOK_VALUE);
    init_param(params[c++], OPEN_FILE);
    init_param(params[c++], SEMI_OPEN_FILE);
    init_param(params[c++], FRIENDLY_FILE);
    init_param(params[c++], FRIENDLY_RANK);
    for(int i = 0;i < 15;i++)  init_param(params[c++], ROOK_MOBILITY[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++] , ROOK_PSQT[i]);

    // Queen eval 
    init_param(params[c++], QUEEN_VALUE);
    for(int i = 0;i < 28;i++)  init_param(params[c++], QUEEN_MOBILITY[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], QUEEN_PSQT[i]);

    // King eval
    for(int i = 0;i <  4;i++) init_param(params[c++], PAWN_SHIELD[i]);
    for(int i = 0;i < 64;i++) init_param(params[c++], KING_PSQT[i]);
    for(int i = 0;i < 100;i++) init_param(params[c++], KING_SAFETY_TABLE[i]);

    // Misc eval
    init_param(params[c++], CONTROL);

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

    for (int i = 0; i < entry->ntuples; i++) 
    {
        midgame += (double) entry->tuples[i].coeff * params[entry->tuples[i].index][MG];
        endgame += (double) entry->tuples[i].coeff * params[entry->tuples[i].index][EG];
    }

    double mixed = ((midgame * (256 - entry->phase)) + (endgame * entry->phase * entry->scale)) / 256;

    return mixed;
}

void update_single_gradient(TPos *entry, TVector gradient, TVector params) 
{
    double E = linear_evaluation(entry, params);
    double S = sigmoid(E);
    double X = (entry->result - S) * S * (1 - S);

    double mg_base = X * entry->pfactors[MG];
    double eg_base = X * entry->pfactors[EG];

    for (int i = 0; i < entry->ntuples; i++) 
    {
        auto index = entry->tuples[i].index;
        auto coeff = entry->tuples[i].coeff;

        gradient[index][MG] += mg_base * coeff;
        gradient[index][EG] += eg_base * coeff * entry->scale;
    }
}

void compute_gradient(TPos *entries, TVector gradient, TVector params)
{
    #pragma omp parallel shared(gradient)
    {
        TVector local = {0};
        #pragma omp for schedule(static, CHUNK)
        for (int i = 0; i < NPOSITIONS; i++)
            update_single_gradient(&entries[i], local, params);

        for (int i = 0; i < NTERMS; i++) 
        {
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
        #pragma omp for schedule(static, CHUNK) reduction(+:total)
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

void print_single(TVector params, std::string_view name, std::ostream& o, int& offset)
{
    o << "\nconstexpr int " << name << " = " << print_param(params[offset]) << ";\n";
    offset++;
}

void print_array(TVector params, int count, std::string_view name, std::ostream& o, int& offset)
{
    o << "\nconstexpr int " << name << "[" << count << "]\n{";

    for(int i = 0;i < count;i++)
    {
        if (i % 8 == 0)
            o << "\n    ";
        
        o << print_param(params[offset + i]) << ", ";
    }
    o << "\n};\n";
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

    fil <<
    R"~(
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
#include "misc.h"

#define mg_score(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define eg_score(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))
)~";

    print_single(tparams, "PAWN_VALUE", fil, c);
    print_single(tparams, "PAWN_STACKED", fil, c);
    print_single(tparams, "PAWN_ISOLATED", fil, c);
    print_single(tparams, "SUPPORTED_PASSER", fil, c);
    print_single(tparams, "PASSER_TEMPO", fil, c);
    print_single(tparams, "PAWN_SUPPORT", fil, c);
    print_array(tparams, 8, "PHALANX", fil, c);
    print_array(tparams, 64, "PAWN_PSQT", fil, c);
    print_array(tparams, 64, "PASSER", fil, c);
    print_array(tparams, 64, "BLOCKED_PASSER", fil, c);

    print_single(tparams, "KNIGHT_VALUE", fil, c);
    print_array(tparams, 9, "KNIGHT_MOBILITY", fil, c);
    print_array(tparams, 64, "KNIGHT_PSQT", fil, c);

    print_single(tparams, "BISHOP_VALUE", fil, c);
    print_array(tparams, 14, "BISHOP_MOBILITY", fil, c);
    print_array(tparams, 64, "BISHOP_PSQT", fil, c);

    print_single(tparams, "ROOK_VALUE", fil, c);
    print_single(tparams, "OPEN_FILE", fil, c);
    print_single(tparams, "SEMI_OPEN_FILE", fil, c);
    print_single(tparams, "FRIENDLY_FILE", fil, c);
    print_single(tparams, "FRIENDLY_RANK", fil, c);
    print_array(tparams, 15, "ROOK_MOBILITY", fil, c);
    print_array(tparams, 64, "ROOK_PSQT", fil, c);

    print_single(tparams, "QUEEN_VALUE", fil, c);
    print_array(tparams, 28, "QUEEN_MOBILITY", fil, c);
    print_array(tparams, 64, "QUEEN_PSQT", fil, c);

    print_array(tparams, 4, "PAWN_SHIELD", fil, c);
    print_array(tparams, 64, "KING_PSQT", fil, c);
    print_array(tparams, 100, "KING_SAFETY_TABLE", fil, c);

fil << 
R"~(
constexpr int KING_ATTACK_WEIGHT[5]
{
    0, 2, 2, 3, 5
};
)~";

    print_single(tparams, "CONTROL", fil, c);

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
    double error, rate = 0.1;

    TPos* entries = (TPos*)calloc(NPOSITIONS, sizeof(TPos));  

    init_tuner_entries(entries);
    init_base_params(current_params);
    save_params(current_params, params);

    for (int epoch = 1; epoch <= 10000; epoch++) 
    {
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
        std::cout << "\rEpoch " << epoch << " Error " << std::fixed << std::setprecision(8) << error;

        if (epoch % 8 == 0) save_params(params, current_params);    
    }
}
#endif