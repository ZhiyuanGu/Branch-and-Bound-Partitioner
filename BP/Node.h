#include<vector>
#include<set>
#include<algorithm>
#include<iostream>
#include<cmath>

using namespace std;

class cut {
	// net number
	int number;
	// whether the net is sure to create a cut
	bool IsCut;
	// whether already in any of the four chunks: left, centre left, centre right, right
	bool left, c_left, c_right, right;
	// if the net is in
public:
	cut(int n) :number(n) { IsCut = false; left = false; c_left = false; c_right = false; right = false; }
	// constructor for those nets that are bound to create a cut in the beginning
	cut(int n, bool ini_cut) : number(n), IsCut(ini_cut) {}
	int getNumber() { return number; }
	bool getIsCut() const { return IsCut; }
	bool NetIntoChunk(int chunk);// legal chunk = 0/1/2/3
	// define natural order
	bool operator<(const cut& c1) const {
		return (number < c1.number);
	}
};

class Node {
private:
	// how many cells have been partitioned
	unsigned int depth;
	// max chunk size limited by balance requirement
	unsigned int MaxChunkSize;
	// four chunks: left, centre left, centre right, right
	vector<int> left, c_left, c_right, right;
	// store information for fast cut set and lower bound calculation
	set<cut> cut_set;
	// how many cuts are already present given the partial partition now
	unsigned int cut_size;
	// position of the node for graphics
	float x, y;
	// lower bound value calculated considering future blocks
	unsigned LB;

public:
	Node();
	Node(unsigned int size);
	// define natural order
	bool operator<(const Node& n1) const {
		return (cut_size < n1.cut_size);
	}
	// define order for priority queue
	friend class LBF;
	// return how many chunks are not empty
	int getOccupancy();
	unsigned int getCutSize() { return cut_size; }
	unsigned int getDepth() { return depth; }
	float getX() { return x; }
	float getY() { return y; }
	unsigned int getLB() { return LB; }
	void setLB(unsigned int lb) { LB = lb; }
	void setXY(float x1, float y1) { x = x1; y = y1; }
	// methods to enable branching
	void updatePosition(int chunk, float drawRange1);
	void updatePosition2(int chunk, float drawRange1);
	void incrementCutSize() { cut_size++; }
	void incrementDepth() { depth++; }
	void BlockIntoChunk(int chunk, int block_num);
	void updateCut(int chunk, int net_num);
	void iniCut(int net_num);

	// cut size lower bound function
	unsigned int LowerBound();
	set<cut>& getCutSet() { return cut_set; }
	// balance violation check before branching
	bool balanceViolation(int chunk);
	// print node
	void print();

};

// auxilary class to define order for priority queue
class LBF {
public:
	bool operator()(const Node& n1, const Node& n2) {
		return n1.LB > n2.LB;
	}
};
