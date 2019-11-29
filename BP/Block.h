#include <iostream>
#include <vector>

using namespace std;

class Block {
private:
	int number;
	bool cared;
	vector<int> connNet;
	int fanout;
	int netCount;

public:
	Block(int n);
	~Block();

	vector<int>& getConnNet() { return connNet; }
	void setFanout(int f) { fanout = f; }
	void incrementNetCount() { netCount++; }
	void setCare() { cared = true; }
	bool isCared() const { return cared; }
	int getFanout() const { return fanout; }
	int getNetCount() const { return netCount; }
	int getNumber() const { return number; }
	vector<int> getConn() { return connNet; }
	void addConn(int N);

};

bool caredFirst(const Block&, const Block&);
bool fanoutFirst(const Block&, const Block&);
bool realFanoutFirst(const Block&, const Block&);

