#include "searchinit.h"
#include "search.h"

void SearchInit::begin(Search& search, Position& position, TTable& tt)
{
	if (worker.joinable())
	{
		end();
	}

	std::cout << "Started search\n";
	using std::ref;
	worker = std::thread(search_position, ref(position), search, ref(tt));
}

void SearchInit::end()
{
	SEARCH_ABORT = true;
	worker.join();
}