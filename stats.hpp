#ifndef __STATS_HPP__
#define __STATS_HPP_

#include <iostream>
struct Stats
{
	size_t branches;
	size_t constrained_by_record;
	void clear()
	{
		branches = 0;
		constrained_by_record = 0;
	}
};

/*void print_stats()
{
	std::cout << "Statistics: \n";
	std::cout << "Branches: " << stats.branches << std::endl;
	std::cout << "Constrained by record: " << stats.constrained_by_record << std::endl;
}
*/
#endif //__STATS_HPP_
