#pragma once
#include "bitboard.h"
#include "piece.h"
#include <string_view>

class CastleRights
{
public:
	CastleRights();

	void reset();

	bool parse_fen(std::string_view);

	uint64_t get_rooks(Color) const;

	void update(Move);

	uint64_t data() const { return rooks; }

	static bool castle_path_is_clear(Square rook, uint64_t);
	static uint64_t get_castle_path(Square);

	friend std::ostream& operator<<(std::ostream&, const CastleRights);
private:
	bool set(const char);

private:
	uint64_t rooks;
};

