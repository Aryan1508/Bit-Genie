#include "search.h"
#include "movegen.h"
#include "position.h"
#include "eval.h"
#include "moveorder.h"
#include "tt.h"
#include <sstream>
#include <unordered_map>



namespace
{
	std::unordered_map<int, int> cutoff_table;

	enum
	{
		MinEval = -std::numeric_limits<int>::max(),
		MaxEval = -MinEval,
		MateEval = MaxEval - 1,
		MaxPly = 64,
		MinMateScore = MateEval - MaxPly,
	};

	struct SearchResult
	{
		int      score = MinEval;
		Move best_move = NullMove;

		SearchResult() = default;

		SearchResult(int best_score, Move best = NullMove)
			: score(best_score), best_move(best)
		{}
	};

	int qsearch(Position& position, Search& search, TTable& tt, int alpha, int beta)
	{
		if (search.limits.stopped)
			return 0;

		search.info.nodes++;

		if ((search.info.nodes & 2047) == 0)
			search.limits.update();

		search.info.update_seldepth();

		if (search.info.ply >= MaxPly)
			return eval_position(position);

		if (position.history.is_drawn(position.key))
			return 0;

		int stand_pat = eval_position(position);

		if (stand_pat >= beta)
			return beta;

		alpha = std::max(alpha, stand_pat);

		SearchResult result;
		MoveGenerator<true> gen;
		gen.generate<MoveGenType::noisy>(position);

		sort_qmovelist(gen.movelist, position, search);

		for (auto move : gen.movelist)
		{
			if (move_score(move) < 0)
				break;

			position.apply_move(move);
			search.info.ply++;
			int score = -qsearch(position, search, tt, -beta, -alpha);
			search.info.ply--;
			position.revert_move();

			if (search.limits.stopped)
				return 0;

			alpha = std::max(alpha, score);

			if (alpha >= beta)
				return beta;
		}

		return alpha;
	}

	SearchResult negamax(Position& position, Search& search, TTable& tt,
						 int depth, int alpha = MinEval, int beta = MaxEval)
	{
		if (search.limits.stopped)
			return 0;

		search.info.total_nodes++;
		search.info.nodes++;

		if ((search.info.nodes & 2047) == 0)
			search.limits.update();

		search.info.update_seldepth();

		if (depth <= 0)
			return qsearch(position, search, tt, alpha, beta);

		if (search.info.ply >= MaxPly)
			return eval_position(position);

		if ((position.history.is_drawn(position.key) || position.half_moves >= 100) && search.info.ply)
			return 0;

		SearchResult result;
	
		int move_num = 0;
		int original = alpha;

		if (search.limits.stopped)
			return 0;

		MovePicker picker(position, search, tt);

		for (Move move; picker.next(move);)
		{
			move_num++;
			position.apply_move(move);
			search.info.ply++;

			int score = 0;
			if (move_num == 1)
				score = -negamax(position, search, tt, depth - 1, -beta, -alpha).score;
			else
			{
				score = -negamax(position, search, tt, depth - 1, -alpha - 1, -alpha).score;

				if (alpha < score && score < beta)
					score = -negamax(position, search, tt, depth - 1, -beta, -score).score;
			}

			search.info.ply--;
			position.revert_move();


			if (search.limits.stopped)
				return 0;

			if (score > result.score)
			{
				result.best_move = move;
				result.score = score;
			}

			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				search.info.total_cutoffs++;
				cutoff_table[move_num]++;
				if (!move_is_capture(position, move))
				{
					search.history.add(position, move, depth);
					search.killers.add(search.info.ply, move);
				}

				return { beta, result.best_move };
			}
		}

		if (move_num == 0)
		{
			if (position.king_in_check())
				return search.info.ply - MateEval;

			else
				return 0;
		}

		if (original != alpha)
		{
			tt.add(position, result.best_move);
		}

		return { alpha, result.best_move };
	}

	int mate_distance(int score)
	{
		if (score > 0)
		{
			return (MateEval - score) / 2 + 1;
		}
		else
		{
			return (MateEval + score) / 2 + 1;
		}
	}

	std::string print_score(int score)
	{
		std::stringstream o;
		if (score > MinMateScore || score < -MinMateScore)
		{
			o << "mate " << mate_distance(score);
		}
		else
		{
			o << "cp " << score / get_score(wPawn);
		}
		return o.str();
	}

	std::vector<Move> get_pv(Position position, TTable& tt, int depth)
	{
		std::vector<Move> pv;

		TEntry* entry = &tt.retrieve(position);

		while (entry->hash == position.key.data() && depth)
		{
			if (position.move_exists(entry->move))
			{
				position.apply_move(entry->move);
				pv.push_back(entry->move);
				depth--;
			}
			else
				break;

			entry = &tt.retrieve(position);
		}

		return pv;
	}

	void print_info_string(Position& position, SearchResult& result, TTable& tt, Search& search, int depth)
	{
		using namespace std::chrono;
		std::printf("info depth %d seldepth %d nodes %llu score %s time %lld pv ",
					depth,
					search.info.seldepth,
					search.info.nodes,
					print_score(result.score).c_str(),
					duration_cast<milliseconds>(search.limits.stopwatch.elapsed_time()).count());

		for (auto m : get_pv(position, tt, depth))
		{
			std::cout << print_move(m) << ' ';
		}

		std::cout << std::endl;
	}
}

void print_cutoffs(Search& search)
{
	for (int i = 1; i < 11; i++)
	{
		int total = cutoff_table[i];
		std::cout << "Move " << i << ": " << std::fixed << (float(total) / search.info.total_cutoffs) * 100 << std::endl;
	}
	std::cout << '\n';
}

void search_position(Position& position, Search& search, TTable& tt)
{
	cutoff_table.clear();
	tt.reset();

	Move best_move = NullMove;
	for (int depth = 1;
		 depth <= search.limits.max_depth;
		 depth++)
	{
		search.info.ply = 0;
		search.info.nodes = 0;

		auto result = negamax(position, search, tt, depth);

		if (search.limits.stopped)
		{
			if (depth == 1)
				std::cout << "stopped at depth 1\n";
			break;
		}

		print_info_string(position, result, tt, search, depth);
		best_move = tt.retrieve(position).move;
	}
	std::cout << "bestmove " << print_move(best_move) << std::endl;
}