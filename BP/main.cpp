#include <iostream>
#include "draw.h"
#include <string>

using namespace std;

int main(void)
{
	Partitioning* BABP = new Partitioning;
	BABP->A3();

	openGraphics(BABP);

	delete BABP;

	return 0;
}

