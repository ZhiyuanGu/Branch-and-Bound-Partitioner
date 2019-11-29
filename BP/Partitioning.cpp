#include "Partitioning.h"

Partitioning::Partitioning() {
	visited = 0;
	best = 32767;
	CaredNumber = 0;
	drawRange = 900.0;
}


Partitioning::~Partitioning() {
}

void Partitioning::A3() {
	cout << "Branch & Bound partitioning starting...\n";
	// input routing parameters filepath
	cout << "Please enter file path(cct1, etc.):\n";
	getline(cin, filePath);

	// input way of traversal
	cout << "Please enter traversal type:\n";
	cout << "(1: Breadth First Traversal, 2: Lower Bound First Traversal)\n";
	int traversalType;
	cin >> traversalType;


	inputFile();

	// start point to record running time
	clock_t start_time = clock();

	// find initial best
	distinctBest();

	if (traversalType == 1)
		BFT();
	else if (traversalType == 2)
		LBFT();

	// end point to record running time
	clock_t end_time = clock();

	printBest();
	//printPosition();


	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Total running time is: " << static_cast<double>(end_time - start_time) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;



}

// input block and net information from file
void Partitioning::inputFile() {
	ifstream infile;
	infile.open(filePath.c_str(), ifstream::in);
	int bn, nn;
	while (infile >> bn && bn != -1) {
		unsigned int i;
		bool found_flag = false;
		// check for duplicated block
		for (i = 0; i < Blocks.size(); i++) {
			if (Blocks[i].getNumber() == bn) {
				found_flag = true;
				break;
			}
		}
		if (!found_flag) {
			// store new block
			Block block(bn);
			Blocks.push_back(block);
			i = Blocks.size() - 1;
		}

		while (infile >> nn && nn != -1) {
			Blocks[i].addConn(nn);
			// also store nets
			found_flag = false;
			unsigned int j;
			// check for duplicated nets
			for (j = 0; j < Nets.size(); j++) {
				if (Nets[j].getNumber() == nn) {
					found_flag = true;
					break;
				}
			}
			if (!found_flag) {
				// store new net
				Net net(nn);
				Nets.push_back(net);
				j = Nets.size() - 1;
			}
			Nets[j].addConn(bn);

		}
	}

	infile.close();

	chunkSize = (Blocks.size() + 3) / 4;

}

// test function to print out the input information
void Partitioning::printBlocks() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out blocks and connected nets...\n";
	//cout << "size:" << Blocks.size() << endl;
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		vector<int> conn = Blocks[i].getConn();
		cout << "Block No." << Blocks[i].getNumber() << " Conn: ";
		for (unsigned int j = 0; j < conn.size(); j++) {
			cout << conn[j] << " ";
		}
		cout << endl;
	}
}

// test function to print out cared blocks information
void Partitioning::printCaredBlocks() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out cared blocks...\n";
	for (unsigned int i = 0; i < CaredBlocks.size(); i++) {
		vector<int> conn = CaredBlocks[i].getConn();
		cout << "Block No." << CaredBlocks[i].getNumber() << " Conn: ";
		for (unsigned int j = 0; j < conn.size(); j++) {
			cout << conn[j] << " ";
		}
		cout << endl;
	}
}

// test function to print out the input information
void Partitioning::printNets() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out nets and connected blocks...\n";
	for (unsigned int i = 0; i < Nets.size(); i++) {
		vector<int> conn = Nets[i].getConn();
		cout << "Net No." << Nets[i].getNumber() << " Conn: ";
		for (unsigned int j = 0; j < conn.size(); j++) {
			cout << conn[j] << " ";
		}
		cout << endl;
	}

}

// greedy DFS for the initial best
void Partitioning::greedyBest() {
	Node N = root();
	setCaredBlocks();
	fanoutSorting();
	vector<Node> fork;
	for (unsigned int i = 0; i < CaredBlocks.size(); i++) {
		for (int c = 0; c < 4; c++) {// c indicates chunk
			int validSize = 0;
			if (!N.balanceViolation(c)) {// if balance is not violated
				validSize++;
				fork.resize(validSize, Node(0));
				fork[validSize - 1] = branch(c, CaredBlocks[i].getNumber(), N);
			}

		}
		// sort to find child with least cut size (local optima)
		sort(fork.begin(), fork.end());
		N = fork[0];
	}
	best = N.getCutSize();
	bestNode = N;
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Initial best achieved by greedy search: " << best << endl;
	N.print();
}

// find distinct cared nets to put into chunks to get initial best
void Partitioning::distinctBest() {
	// find at most 2 distinct cared nets
	// increase number of cared net distict with each other would increase complexity greatly
	// find cared nets
	setCaredNets();
	// find distinct cared nets
	bool found = false;
	// array to store which block should be put into which chunk
	// pair<chunk, blockNumber>
	vector< pair<int, int> > blockChunk;
	// initialize block and chunk
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		// 5 means the block not designated a chunk
		blockChunk.push_back(pair<int, int>(5, i + 1));
	}
	for (vector<Net>::iterator cn1_it = CaredNets.begin(); cn1_it < CaredNets.end(); cn1_it++) {
		for (vector<Net>::iterator cn2_it = cn1_it + 1; cn2_it < CaredNets.end(); cn2_it++) {
			set<int> distinctBlocks;
			// insert all blocks in the two net
			for (unsigned int bn = 0; bn < cn1_it->getConn().size(); bn++) {
				int bn1 = cn1_it->getConn()[bn];
				distinctBlocks.insert(bn1);
			}
			for (unsigned int bn = 0; bn < cn2_it->getConn().size(); bn++) {
				int bn2 = cn2_it->getConn()[bn];
				distinctBlocks.insert(bn2);
			}
			if (distinctBlocks.size() == cn1_it->getConn().size() + cn2_it->getConn().size()) {
				// if the two nets are distinct
				found = true;
				// set block chunk
				for (unsigned int bn = 0; bn < cn1_it->getConn().size(); bn++) {
					int bn1 = cn1_it->getConn()[bn];
					blockChunk[bn1 - 1] = pair<int, int>(3, bn1);
				}
				for (unsigned int bn = 0; bn < cn2_it->getConn().size(); bn++) {
					int bn2 = cn2_it->getConn()[bn];
					blockChunk[bn2 - 1] = pair<int, int>(2, bn2);
				}
				break;
			}

		}
		if (found)
			break;
	}
	if (!found) {// if not found, put one net in one chunk
		for (unsigned int bn = 0; bn < CaredNets[0].getConn().size(); bn++) {
			int bn1 = CaredNets[0].getConn()[bn];
			blockChunk[bn1 - 1] = pair<int, int>(3, bn1);
		}
	}
	// sort to put distinct cared net in the front
	sort(blockChunk.begin(), blockChunk.end());
	// put distinct net blocks into different chunks first
	Node N = root();
	setCaredBlocks();
	fanoutSorting();
	vector<Node> fork;
	for (vector< pair<int, int> >::iterator i = blockChunk.begin(); i < blockChunk.end(); i++) {
		pair<int, int> x = *i;
		for (int c = 0; c < 4; c++) {// c indicates chunk
			if (x.first < 4) {
				fork.resize(1, Node(0));
				fork[0] = branch(x.first, x.second, N);
				break;
			}
			int validSize = 0;
			if (!N.balanceViolation(c)) {// if balance is not violated
				validSize++;
				fork.resize(validSize, Node(0));
				fork[validSize - 1] = branch(c, x.second, N);
			}

		}
		if (x.first >= 4) {
			// sort to find child with least cut size (local optima)
			sort(fork.begin(), fork.end());
		}
		N = fork[0];
	}
	best = N.getCutSize();
	bestNode = N;
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Initial solution achieved by prearrange distinct cared nets: " << best << endl;
	N.print();
}

// create root node
Node Partitioning::root() {
	if (Blocks.size() % 4 != 0) {
		cout << "Error: input cells not dividable by 4\n";
	}
	Node root(chunkSize);
	root.setXY(drawRange / 2, 0);

	return root;
}

// mark initial cuts in the node (conn size > max chunk size)
Node Partitioning::initialBound(Node root) {
	CaredBlocks = Blocks;
	for (unsigned int i = 0; i < Nets.size(); i++) {
		Net x = Nets[i];
		if (x.getConn().size() > chunkSize) {
			// net size larger than chunk size is always creating a cut
			root.incrementCutSize();
			root.iniCut(x.getNumber());
		}

	}
	return root;
}

// create CaredBlocks and count cared block number
void Partitioning::setCaredBlocks() {
	// apply net information to determine cuts early
	CaredBlocks = Blocks;
	for (unsigned int i = 0; i < Nets.size(); i++) {
		Net x = Nets[i];
		if (x.getConn().size() <= chunkSize) {
			// only consider the blocks that actually could affect cut size
			for (unsigned int i = 0; i < x.getConn().size(); i++) {
				int b = x.getConn()[i];
				if (!CaredBlocks[b - 1].isCared()) {
					CaredBlocks[b - 1].setCare();
				}
				// calculate the actual relevant net count
				CaredBlocks[b - 1].incrementNetCount();
			}

		}
	}

	// put cared blocks to the front
	sort(CaredBlocks.begin(), CaredBlocks.end(), caredFirst);
	// record cared block number
	CaredNumber = 0;
	for (unsigned int i = 0; i < CaredBlocks.size(); i++) {
		Block B = CaredBlocks[i];
		if (B.isCared())
			CaredNumber++;
	}
}

// create CaredNets from Nets
void Partitioning::setCaredNets() {
	CaredNets.clear();
	for (unsigned int i = 0; i < Nets.size(); i++) {
		if (Nets[i].getConn().size() <= chunkSize) {
			CaredNets.push_back(Nets[i]);
		}
	}
}

// should happen after setting cared blocks, modify CaredBlocks order
void Partitioning::fanoutSorting() {
	// sort nets according to number
	sort(Nets.begin(), Nets.end());
	// assign fanout value for Cared blocks
	for (vector<Block>::iterator it = CaredBlocks.begin(); it < CaredBlocks.begin() + CaredNumber; it++) {// all cared blocks
		set<int> blocksInConnNet;
		for (unsigned int i = 0; i < it->getConn().size(); i++) {// all connected cared nets
			int n = it->getConn()[i];
			if (Nets[n - 1].getConn().size() <= chunkSize) {
				for (unsigned int j = 0; j < Nets[n - 1].getConn().size(); j++) {
					int b = Nets[n - 1].getConn()[j];
					blocksInConnNet.insert(b);
				}
			}
		}
		it->setFanout(blocksInConnNet.size());
	}

	// fanout sorting according to no. of distinctive blocks connected to the block
	sort(CaredBlocks.begin(), CaredBlocks.begin() + CaredNumber, realFanoutFirst);
}

// should happen after balance violation check
Node Partitioning::branch(int chunk, int block_num, const Node& oldNode) {
	// default copy constructor
	Node newNode = oldNode;
	// first:update position
	newNode.updatePosition2(chunk, drawRange);
	// update depth
	newNode.incrementDepth();
	// update chunks, given balance requirement not violated
	newNode.BlockIntoChunk(chunk, block_num);
	// update cut set
	for (unsigned int i = 0; i < Blocks[block_num - 1].getConn().size(); i++) {
		int n = Blocks[block_num - 1].getConn()[i];
		newNode.updateCut(chunk, n);
	}
	// return the new node
	return newNode;
}

// calculate the lower bound of cut size given the current cut size and prediction into future blocks/nets
unsigned int Partitioning::LowerBound(Node& N) {
	unsigned int Depth = N.getDepth();
	unsigned int cutSize = N.getCutSize();
	// find a common unpartitioned block in two cared nets which havenot created a cut
	// find all cared nets
	setCaredNets();
	// erase the nets that have already created cut in the node
	int shouldIncrement;
	int i = 0;
	while (i < CaredNets.size()) {
		shouldIncrement = 1;
		// temporary object for finding
		cut cut_temp(CaredNets[i].getNumber());
		set<cut>::iterator it_find = N.getCutSet().find(cut_temp);
		if ((it_find != N.getCutSet().end()) && (it_find->getIsCut())) {// cut is created already
			CaredNets.erase(CaredNets.begin() + i);
			// acount for iterator change after erasing
			shouldIncrement = 0;
		}
		i += shouldIncrement;
	}
	//find two different cared nets without current cuts
	for (vector<Net>::iterator it1 = CaredNets.begin(); it1 < CaredNets.end(); it1++) {
		bool found = false;
		for (vector<Net>::iterator it2 = it1 + 1; it2 < CaredNets.end(); it2++) {
			// check if they have common block that are not partitioned yet			
			for (unsigned int i1 = 0; i1 < it1->getConn().size(); i1++) {
				for (unsigned int i2 = 0; i2 < it2->getConn().size(); i2++) {
					int n1 = it1->getConn()[i1];
					int n2 = it2->getConn()[i2];
					if (n1 == n2) {
						// check if the block hasnot been partitioned
						for (unsigned int bi = Depth; bi < CaredBlocks.size(); bi++) {
							if (n1 == CaredBlocks[bi].getNumber()) {
								found = true;
								break;
							}
						}
					}
					if (found)
						break;
				}
				if (found)
					break;
			}
			if (found) {
				set<int> block_set;
				for (unsigned int i1 = 0; i1 < it1->getConn().size(); i1++) {
					int n1 = it1->getConn()[i1];
					block_set.insert(n1);
				}
				for (unsigned int i2 = 0; i2 < it2->getConn().size(); i2++) {
					int n2 = it2->getConn()[i2];
					block_set.insert(n2);
				}
				if (block_set.size() > chunkSize) {
					cutSize++;
					CaredNets.erase(it2);
					break;
				}

			}
		}
		if (found) {
			if ((it1 + 1) < CaredNets.end())
				continue;
			else
				break;
		}
	}

	return cutSize;
}

// breadth first traversal algorithm
void Partitioning::BFT() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Starting Branch & Bound Partitioning - Breadth First Traversal: " << endl;
	// queue of nodes for branching in BFT
	queue<Node> Nodes;
	Node N = initialBound(root());
	parentPos.push_back(pair<float, float>(N.getX(), N.getY()));//record parent node position
	position.push_back(pair<float, float>(N.getX(), N.getY()));//record position
	if (LowerBound(N) < best) {
		Nodes.push(N);
	}
	else {
		cout << "The decision tree is pruned at its root!\n";
		cout << "Estimated lower bound of cut size: " << LowerBound(N) << endl;
		cout << "Initial best cut size: " << best << endl;
	}
	visited++;
	// set cared blocks and sort in descending fanout(number of nets) order
	setCaredBlocks();
	fanoutSorting();
	int d_record = 0;
	while (!Nodes.empty()) {
		N = Nodes.front();//read parent node
		Nodes.pop();
		int Depth = N.getDepth();
		// test to print current tree depth of BFT
		if (d_record != Depth) {
			cout << "Current deepest depth = " << Depth + 1 << endl;
			d_record = Depth;
		}
		if (Depth == CaredNumber) {// if finished partitioning cared blocks, break to go down stairs
			cout << "Finished partitioning cared blocks!!\n";
			break;
		}
		//int chunk_limit = min(4, (Depth + 1));// first edition of symmetry
		int chunk_limit = min(4, (N.getOccupancy() + 1));// second edition of symmetry
		for (int c = 0; c < chunk_limit; c++) {// c indicates chunk
			visited++;// detecting whether the node is balanced means it is visited
			if (!N.balanceViolation(c)) {// bound: if balance is violated, prune
				parentPos.push_back(pair<float, float>(N.getX(), N.getY()));//record parent node position
				Node N_child = branch(c, CaredBlocks[Depth].getNumber(), N);// branch to next layer
				position.push_back(pair<float, float>(N_child.getX(), N_child.getY()));//record position
				if (Depth + 1 == CaredNumber) {// node is leaf(tree of cared blocks)
					if (N_child.getCutSize() < best) {// if reult is better, update best
						best = N_child.getCutSize();
						bestNode = N_child;
						// test: print best
						cout << "Achieved best node with just going through the cared blocks: " << endl;
						bestNode.print();
					}
				}
				else if (LowerBound(N_child) < best) {// node is not leaf and dont have estimated larger cut size than current optimal
					Nodes.push(N_child);
				}
			}
		}
	}
	// partition the don't-care blocks(just like going down stairs)
	while (bestNode.getDepth() < CaredBlocks.size()) {// early termination due to dont-care blocks
		int Depth = bestNode.getDepth();
		for (int c = 0; c < 4; c++) {// c indicates chunk
			visited++;// detecting whether the node is balanced means it is visited
			if (!bestNode.balanceViolation(c)) {// bound: if balance is violated, change chunk to insert
				parentPos.push_back(pair<float, float>(bestNode.getX(), bestNode.getY()));//record parent node position
				bestNode = branch(c, CaredBlocks[Depth].getNumber(), bestNode);// branch to next layer
				position.push_back(pair<float, float>(bestNode.getX(), bestNode.getY()));//record position
				// simply pick the first available slot for don't-care blocks
				break;

			}
		}

	}


}

// lower bound first traversal algorithm
void Partitioning::LBFT() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Starting Branch & Bound Partitioning - Lower Bound First Traversal: " << endl;
	// priority queue of nodes for branching in LBFT
	priority_queue<Node, vector<Node>, LBF> Nodes;
	Node N = initialBound(root());
	parentPos.push_back(pair<float, float>(N.getX(), N.getY()));//record parent node position
	position.push_back(pair<float, float>(N.getX(), N.getY()));//record position
	// calculate lower bound of the node
	N.setLB(LowerBound(N));
	if (N.getLB() < best) {
		Nodes.push(N);
	}
	else {
		cout << "The decision tree is pruned at its root!\n";
		cout << "Estimated lower bound of cut size: " << LowerBound(N) << endl;
		cout << "Initial best cut size: " << best << endl;
	}
	visited++;
	// set cared blocks and sort in descending fanout(number of nets) order
	setCaredBlocks();
	fanoutSorting();
	//for test
	int d_record = 0;
	while (!Nodes.empty()) {
		N = Nodes.top();//read parent node
		Nodes.pop();
		int Depth = N.getDepth();
		// test to print current largest tree depth of LBFT
		if (d_record < Depth) {
			cout << "Current deepest depth = " << Depth + 1 << endl;
			d_record = Depth;
		}
		//int chunk_limit = min(4, (Depth + 1));// first edition of symmetry
		int chunk_limit = min(4, (N.getOccupancy() + 1));// second edition of symmetry
		for (int c = 0; c < chunk_limit; c++) {// c indicates chunk
			visited++;// detecting whether the node is balanced means it is visited
			if (!N.balanceViolation(c)) {// bound: if balance is violated, prune
				parentPos.push_back(pair<float, float>(N.getX(), N.getY()));//record parent node position
				Node N_child = branch(c, CaredBlocks[Depth].getNumber(), N);// branch to next layer
				position.push_back(pair<float, float>(N_child.getX(), N_child.getY()));//record position
				if (Depth + 1 == CaredNumber) {// node is leaf(tree of cared blocks)
					if (N_child.getCutSize() < best) {// if reult is better, update best
						best = N_child.getCutSize();
						bestNode = N_child;
						// test: print best
						cout << "Achieved best node with just going through the cared blocks: " << endl;
						bestNode.print();
					}
				}
				else {
					// calculate lower bound of the node
					N_child.setLB(LowerBound(N_child));
					if (N_child.getLB() < best) {// node is not leaf and prune nodes with larger cut size than current optimal
						Nodes.push(N_child);
					}
				}
			}
		}

	}
	// partition the don't-care blocks(just like going down stairs)
	while (bestNode.getDepth() < CaredBlocks.size()) {// early termination due to dont-care blocks
		int Depth = bestNode.getDepth();
		for (int c = 0; c < 4; c++) {// c indicates chunk
			visited++;// detecting whether the node is balanced means it is visited
			if (!bestNode.balanceViolation(c)) {// bound: if balance is violated, change chunk to insert
				parentPos.push_back(pair<float, float>(bestNode.getX(), bestNode.getY()));//record parent node position
				bestNode = branch(c, CaredBlocks[Depth].getNumber(), bestNode);// branch to next layer
				position.push_back(pair<float, float>(bestNode.getX(), bestNode.getY()));//record position

				// simply pick the first available slot for don't-care blocks
				break;

			}
		}

	}

}

// print the final result
void Partitioning::printBest() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Print Branch and Bound partitioning result...\n";
	bestNode.print();
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Total visited tree node count: " << visited << endl;

}

// print node position
void Partitioning::printPosition() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Print Node Position...\n";
	unsigned int positionSize = position.size();
	//unsigned int positionSize = 30;
	for (unsigned int i = 0; i < positionSize; i++) {
		cout << "( " << position[i].first << ", " << position[i].second << " )\n";
	}
}
