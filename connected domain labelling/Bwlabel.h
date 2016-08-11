// Bwlabel

#ifndef BWLABEL_

#define BWLABEL_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//using namespace std;
class Bwlabel {

public:

	Bwlabel(const std::vector<int>& image, int rows, int cols);
	~Bwlabel();

	//set 4 or 8 connnection. 
	void setOffset(const int offset){ offset_ = offset;}
	// get connection type.
	int getOffset(){ return offset_; }

	int work(std::vector<int>& image);
	//------------------------------
	//functions
	void fillRunVectors( int& NumberOfRuns, vector<int>& stRun, vector<int>& enRun, vector<int>& rowRun);
	void firstPass(const vector<int>& stRun, const vector<int>& enRun, const vector<int>& rowRun,const int NumberOfRuns,
               vector<int>& runLabels, vector<pair<int, int>>& equivalences, const int offset);
	int replaceSameLabel(vector<int>& runLabels,vector<pair<int, int>>& equivalences);
	void getConnectedZone(const int NumberOfRuns,const vector<int>& stRun,const vector<int>& enRun, const vector<int>& rowRun,const vector<int>& runLabels);
	//----------------------- 

	void printMatrix() const;

private: 
	
	int rows_;
	int cols_;
	int **mat_;

	int offset_;

};



#endif