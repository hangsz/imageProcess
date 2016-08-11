#ifndef NLMFILTER_H

#define NLMFILTER_H

#include <vector>
#include <string>

using namespace std;

// non-local means filter

enum  nlmType{ Vectorial = 0, Adaptive, Plugin, Pearson };

class nlmFilter{

public:
	nlmFilter(const float sigma);

	nlmFilter() = delete;

	void nlmTypeSetting(const nlmType type = Vectorial,const bool isAdaptive = true){
		 nlmType_ = type;
		 isAdaptive_ = isAdaptive;
	}

	string getnlmType(const int index) const {
		return nlmTypeList[index];
	}

	bool filter( vector<int>& image,const int width,const int height ) const;

private:

	void traverseXY(const int width, const int height, const vector<float>& inBGR, const vector<float>& midBGR, vector<float>& outBGR) const;
	float distancePQ(const int width, const int height, const vector<float>& inBGR, const vector<float>& midBGR, const int x, const int y, const int i, const int j, const int fatch) const;
	
	void createSSI(const int width, const int height, const vector<float>& inRGB, vector<vector<float>>& SSI) const;
	float distancePQ(const int width, const int height, const vector<float>& inBGR, const int x, const int y, const int i, const int j, const int fatch,const vector<vector<float>>& SSI) const;

	float findInExpLut(const vector<float>& expLut, const float dist) const;

	
	nlmType nlmType_ = Vectorial;

	static const string nlmTypeList[4] ;

	bool isAdaptive_ = true;

	const float sigma_ ;		
	
	int fpatch_;
	int rblock_ ;
	float kfilter_;

	const int interval_ = 2;

	const float lim1 = 0.99f;
	const float lim2 = 0.99f;

	const float weightThreshold = 0.00000001f;
	const int expLutMax = 30;
	const unsigned int expLutPrecision = 1000 ;

};

#endif