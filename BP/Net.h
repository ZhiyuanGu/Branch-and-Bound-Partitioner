#include <iostream>
#include <vector>


using namespace std;

class Net {
private:
	int number;
	vector<int> connBlock;

public:
	Net(int n);
	~Net();


	int getNumber() { return number; }
	vector<int> getConn() { return connBlock; }
	void addConn(int B);

	// define natual order
	bool operator<(const Net& N1)const {
		return (this->number < N1.number);
	}


};
