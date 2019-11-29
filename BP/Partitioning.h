#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <cmath>
#include <time.h>

#include "Block.h"
#include "Net.h"
#include "Node.h"

using namespace std;

class Partitioning {

private:
	string filePath;

	int visited;
	int CaredNumber;
	unsigned int chunkSize;

	unsigned int best;
	Node bestNode;

	// block and net information
	vector<Block> Blocks;
	vector<Block> CaredBlocks;
	vector<Net> Nets;
	vector<Net> CaredNets;
	// for graphics
	double drawRange;
	vector<pair<float, float> > parentPos;
	vector<pair<float, float> > position;

public:
	Partitioning();
	~Partitioning();

	vector<Block>* getBlocks() { return &Blocks; }
	vector<Block>* getCaredBlocks() { return &CaredBlocks; }
	vector<Net>* getNets() { return &Nets; }
	vector<pair<float, float> >* getParentPos() { return &parentPos; }
	vector<pair<float, float> >* getPosition() { return &position; }

	// input from file
	void inputFile();
	void printBlocks();
	void printNets();
	void printCaredBlocks();
	// determine initial best
	void greedyBest();
	void distinctBest();
	// create root node
	Node root();
	Node initialBound(Node root);
	void setCaredBlocks();
	void setCaredNets();
	void fanoutSorting();

	Node branch(int chunk, int block_num, const Node& oldNode);

	unsigned int LowerBound(Node& N);
	// breadth first traversal algorithm
	void BFT();
	// lower bound first traversal algorithm
	void LBFT();
	// print the final result
	void printBest();
	// print node position
	void printPosition();


	// main partition work
	void A3();





};



