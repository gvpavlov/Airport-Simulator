#include "airport.h"

int main()
{
	// Airport default constructor takes:
	// 1. time
	// 2. runway count
	// 3. max number of planes spawning per turn
	Airport example1(10, 5, 5);
	example1.simulation();

	Airport example2(10, 0, 3);
	example2.simulation();
	
	Airport example3(5000, 10, 7);
	example3.simulation();
	
	Airport example4(100000, 15, 12);
	example4.simulation();
	
	Airport example5(15, 4, 3);
	example5.simulation();
	return 0;
}