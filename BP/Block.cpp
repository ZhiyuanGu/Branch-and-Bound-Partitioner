#include "Block.h"

bool caredFirst(const Block& B1, const Block& B2) {
	return (B1.isCared() > B2.isCared()) || ((B1.isCared() == B2.isCared()) && (B1.getNumber() < B2.getNumber()));
}

bool fanoutFirst(Block& B1, Block& B2) {
	return (B1.getConnNet().size() > B2.getConnNet().size());
}

bool realFanoutFirst(const Block& B1, const Block& B2) {
	return (B1.getFanout() > B2.getFanout()) || ((B1.getFanout() == B2.getFanout()) && (B1.getNumber() > B2.getNumber()));
}

Block::Block(int n) {
	number = n;
	cared = false;
	fanout = 0;
	netCount = 0;
}

Block::~Block() {
}


void Block::addConn(int N) {
	bool alreadyExist = false;
	// check for duplicated nets
	for (unsigned int i = 0; i < connNet.size(); i++)
	{
		if (connNet[i] == N)
			alreadyExist = true;
	}
	if (!alreadyExist)
		connNet.push_back(N);
}

