#include "Net.h"

Net::Net(int n) {
	number = n;
}

Net::~Net() {
}


void Net::addConn(int B) {
	bool alreadyExist = false;
	for (unsigned int i = 0; i < connBlock.size(); i++)
	{
		if (connBlock[i] == B)
			alreadyExist = true;
	}
	if (!alreadyExist)
		connBlock.push_back(B);
}



