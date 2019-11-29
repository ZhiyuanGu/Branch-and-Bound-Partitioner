#include "Node.h"

using namespace std;

// return value indicates whether a new cut has been created
bool cut::NetIntoChunk(int chunk) {
	if (IsCut) {
		return true;
	}
	else if (chunk == 0) {
		if (c_left | c_right | right) {
			IsCut = true;
			return true;
		}
		else {
			left = true;
		}
	}
	else if (chunk == 1) {
		if (left | c_right | right) {
			IsCut = true;
			return true;
		}
		else {
			c_left = true;
		}
	}
	else if (chunk == 2) {
		if (c_left | left | right) {
			IsCut = true;
			return true;
		}
		else {
			c_right = true;
		}
	}
	else if (chunk == 3) {
		if (c_left | c_right | left) {
			IsCut = true;
			return true;
		}
		else {
			right = true;
		}
	}
	else {
		cout << "Error: illegal chunk number!\n";
	}
	return false;


}

Node::Node() {
	depth = 0;
	cut_size = 0;
	MaxChunkSize = 0;
}

Node::Node(unsigned int size) {
	depth = 0;
	cut_size = 0;
	MaxChunkSize = size;
	x = 0;
	y = 0;
}

// report how many chunks are occupied with at least one block
int Node::getOccupancy() {
	int l = left.empty();
	int cl = c_left.empty();
	int cr = c_right.empty();
	int r = right.empty();

	return (4 - l - cl - cr - r);
}

// balance violation check before branching
bool Node::balanceViolation(int chunk) {
	switch (chunk) {
	case 0: if (left.size() >= MaxChunkSize) return true; break;
	case 1: if (c_left.size() >= MaxChunkSize) return true; break;
	case 2: if (c_right.size() >= MaxChunkSize) return true; break;
	case 3: if (right.size() >= MaxChunkSize) return true; break;
	default: cout << "Error: illegal chunk number!\n"; return true;
	}
	return false;
}

// first thing to do when branching
void Node::updatePosition(int chunk, float drawRange1) {
	x = x + drawRange1 * (-0.5*pow(0.25, (double)depth) + (chunk + 0.5)*pow(0.25, (double)depth + 1.0));
	y = y + drawRange1 * pow(0.5, depth + 1.0);
}

// first thing to do when branching
void Node::updatePosition2(int chunk, float drawRange1) {
	x = x + drawRange1 * (-0.5*pow(0.25, (double)depth) + (chunk + 0.5)*pow(0.25, (double)depth + 1.0));
	y = y + drawRange1 / (4.0* (float)MaxChunkSize);
}

void Node::BlockIntoChunk(int chunk, int block_num) {
	switch (chunk) {
	case 0:
		left.push_back(block_num);
		break;
	case 1:
		c_left.push_back(block_num);
		break;
	case 2:
		c_right.push_back(block_num);
		break;
	case 3:
		right.push_back(block_num);
		break;
	default: cout << "Error: illegal chunk number!\n";
	}
}

void Node::updateCut(int chunk, int net_num) {
	// create temporary object for finding
	cut cut_temp(net_num);
	set<cut>::iterator find_it = cut_set.find(cut_temp);
	if (find_it == cut_set.end()) {
		// if not found, push the new cut
		cut_temp.NetIntoChunk(chunk);
		cut_set.insert(cut_temp);
	}
	else {
		// if found, check if new cut is created
		cut_temp = *find_it;
		if (!cut_temp.getIsCut()) {
			cut_set.erase(find_it);
			if (cut_temp.NetIntoChunk(chunk)) {
				cut_size++;
			}
			cut_set.insert(cut_temp);
		}
	}
}

void Node::iniCut(int net_num) {
	cut_set.insert(cut(net_num, 1));
}

unsigned int Node::LowerBound() {
	// check for possible destined cut of appeared cared nets

	// check for possible destined cut of cared nets that havenot appeared


	return cut_size;
}

// test function to print out information of a node
void Node::print() {
	cout << "-----------------------------------------------\n";
	cout << "Test: printing out node information...\n";
	cout << "depth of node: " << depth << endl;
	cout << "maximum chunk size: " << MaxChunkSize << endl;
	// chunks
	cout << "left chunk: ";
	for (unsigned int i = 0; i < left.size(); i++) {
		cout << left[i] << " ";
	}
	cout << endl;
	cout << "centre left chunk: ";
	for (unsigned int i = 0; i < c_left.size(); i++) {
		cout << c_left[i] << " ";
	}
	cout << endl;
	cout << "centre right chunk: ";
	for (unsigned int i = 0; i < c_right.size(); i++) {
		cout << c_right[i] << " ";
	}
	cout << endl;
	cout << "right chunk: ";
	for (unsigned int i = 0; i < right.size(); i++) {
		cout << right[i] << " ";
	}
	cout << endl;
	// cuts
	cout << "cut size: " << cut_size << endl;
	cout << "cut created by net: ";
	for (set<cut>::iterator i = cut_set.begin(); i != cut_set.end(); i++) {
		cut x = *i;
		if (x.getIsCut()) {
			cout << x.getNumber() << " ";
		}
	}
	cout << endl;
}
