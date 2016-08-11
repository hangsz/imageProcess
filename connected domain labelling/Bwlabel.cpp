#include "Bwlabel.h"
#include <algorithm>


Bwlabel::Bwlabel(const std::vector<int>& image,int rows, int cols):rows_(rows),cols_(cols),offset_(1){

	mat_ = new int*[rows_];
	for(int i=0;i<rows_;i++)
		mat_[i] = new int[cols_];
	 
	for( int i=0;i<rows_;i++)
	for( int j=0;j<cols_;j++)
		mat_[i][j] = image[i*cols_ + j];
}

Bwlabel::~Bwlabel(){

	for(int i = 0 ; i < rows_ ; i++)
		delete[] mat_[i];

	delete[] mat_;
}

//main caller 
int Bwlabel::work(std::vector<int>& image){

	//printMatrix();
	//cout<<endl;

	//-----------------------------------------
	int NumberOfRuns = 0;  // initialize with 0, no runs.

	vector<int> stRun, enRun, rowRun;

	//void fillRunVectors(int& NumberOfRuns, vector<int>& stRun, vector<int>& enRun, vector<int>& rowRun);
	fillRunVectors( NumberOfRuns, stRun, enRun, rowRun);

    //--------------------------
	
	vector<int> runLabels;
	vector<pair<int,int>> equivalences;

	//setOffset(1);
	int offset = getOffset();

	//void firstPass(const vector<int>& stRun, const vector<int>& enRun, const vector<int>& rowRun, const int NumberOfRuns,
    //           vector<int>& runLabels, vector<pair<int, int>>& equivalences, int offset);
	firstPass(stRun, enRun,rowRun, NumberOfRuns, runLabels, equivalences, offset);


	//void replaceSameLabel(vector<int>& runLabels, const vector<pair<int, int>>& equivalences);
	int labelNum = replaceSameLabel(runLabels, equivalences);


	//void getConnectedZone(const int NumberOfRuns, const vector<int>& stRun, const vector<int>& enRun, const vector<int>& rowRun, const vector<int>& runLabels)
	getConnectedZone( NumberOfRuns, stRun, enRun, rowRun, runLabels);

	//printMatrix();


	image.clear();

	for(int i = 0; i<rows_ ;i++)
	for(int j = 0; j<cols_ ;j++)
		image.push_back(mat_[i][j]);

	return labelNum;

}

void Bwlabel::printMatrix() const{
	cout << "printMatrix" << endl;


	for(int i=0;i< rows_ ; i++){
	    for(int j=0;j<cols_;j++)
		    std::cout<<mat_[i][j]<<'\t';
		std::cout<<std::endl;
	}
}

//fillRunVectors is used to find and record all the runs. 
void Bwlabel::fillRunVectors( int& NumberOfRuns, vector<int>& stRun, vector<int>& enRun, vector<int>& rowRun)
{
	//cout << "findRunVectors" << endl;
    for (int i = 0; i < rows_; i++)
    {
        const int* rowData = mat_[i]; //get the first point of each row.


        // That the first element is 0 means that runs need increase 1.
		// this if can make the following judgement keep consistent.
        if ( 0 == rowData[0] )   
        {
            NumberOfRuns++;
            stRun.push_back(0);
            rowRun.push_back(i);
        }

        for (int j = 1; j < cols_; j++)
        {
            if (255 == rowData[j - 1]   && 0 == rowData[j] ) // find new run.
            {
                NumberOfRuns++;
                stRun.push_back(j);
                rowRun.push_back(i);
            }
            else if ( 0 == rowData[j - 1]  && 255 == rowData[j] )  //a run end. 
            {
                enRun.push_back(j - 1);
            }
        }
        if (0 == rowData[cols_ - 1])  // if a run reaches margin, end it.
        {
            enRun.push_back(cols_ - 1);
        }
    }

}

//firstPass is used to label runs and generate equivalent pairs.
void Bwlabel::firstPass(const vector<int>& stRun, const vector<int>& enRun, const vector<int>& rowRun,const int NumberOfRuns,
               vector<int>& runLabels, vector<pair<int, int>>& equivalences,const int offset)
{
    runLabels.assign(NumberOfRuns, 0); //initialize all the runs with label--0.

    int idxLabel = 1;   // 
    int curRowIdx = 0;  // index of  current row. 
    int firstRunOnCur = 0; // index of the first run in current row, 0 is used fisrtly to mark run--0 in row--0 as the first run in previous row. 
    
	int firstRunOnPre = 0; // index of the first run in previous rows_, 0 is larger than -1.
    int lastRunOnPre = -1; // index of the last  run in previous rows_, -1 is used firstly to skip runs in row--0 .

    for (int i = 0; i < NumberOfRuns; i++) {    // traverse every run.
		// runs on the first row are assigned with label--1 and ignored.
        if (rowRun[i] != curRowIdx) { // a run lacates in a new row, first hits when a run lacate in row--1.
            curRowIdx = rowRun[i];
            firstRunOnPre = firstRunOnCur;
            lastRunOnPre = i - 1;  // runs before current run lacates in previous rows_( not means one before ).
            firstRunOnCur = i;
        }
        for (int j = firstRunOnPre; j <= lastRunOnPre; j++){
            if (stRun[i] <= enRun[j] + offset && enRun[i] >= stRun[j] - offset && rowRun[i] == rowRun[j] + 1){
                if (runLabels[i] == 0) // hasn't been labeled, use the label of run--j to label it. 
                    runLabels[i] = runLabels[j];  
                else if (runLabels[i] != runLabels[j])
					// has been labeled, make pair of label--i and label--j, keep minimum label.            
                    equivalences.push_back(make_pair(runLabels[i], runLabels[j])); 
            }
        }
        if (runLabels[i] == 0) // not overlap with runs in previous one row, the number of lable increase 1.
            runLabels[i] = idxLabel++;

    }


}

//find all the equivalent list, using BFS.
int Bwlabel::replaceSameLabel(vector<int>& runLabels,vector<pair<int, int>>& equivalences){

    int maxLabel = *max_element(runLabels.begin(), runLabels.end()); // the label of last run dosen't the maximum label.
    // describe the graph using adjacent matrix.
	// maxLabel X maxLabel, initialized with bool--false.
	vector<vector<bool>> eqTab(maxLabel, vector<bool>(maxLabel, false));

    vector<pair<int, int>>::iterator vecPairIt = equivalences.begin();// iterator begin.
   
	while (vecPairIt != equivalences.end()) { 
		// traverse every equivalent pair, construct undirected graph.
		eqTab[vecPairIt->first - 1][vecPairIt->second - 1] = true;
        eqTab[vecPairIt->second - 1][vecPairIt->first - 1] = true;
        vecPairIt++;
    }

    // each label belongs to a flag, each flag means a connected zone.
    vector<int> labelFlag(maxLabel, 0); 

    vector<vector<int>> equaList; // equivalent list.
    vector<int> tempList; // used to store current equivalent list.
   // cout << maxLabel << endl;

    for (int i = 1; i <= maxLabel; i++) {
        // a label has been traversed, skip it.
        if (labelFlag[i - 1])  continue;

        //fisrtly assign label--0 with flag--1.
		//new label flag equials the number of equivalent lists plusing 1.
        labelFlag[i - 1] = equaList.size() + 1; 

        tempList.push_back(i); // store the first label in current equivalent list.
		// traverse all the label in current equivalent list to find more equivalent label.
        for (vector<int>::size_type j = 0; j < tempList.size(); j++) {
			// traverse all the elements in row--tempList[j] of graph eqTab to find more equivalent label.
            for (vector<bool>::size_type k = 0; k != eqTab[tempList[j] - 1].size(); k++) {
                if (!labelFlag[k] && eqTab[tempList[j] - 1][k] ) {
                    tempList.push_back(k + 1);
                    labelFlag[k] = equaList.size() + 1;
                }
            }
        }
        equaList.push_back(tempList);
        tempList.clear();// cautious!
    }

    // cout << equaList.size() << endl;
	// change the label of each run to the flag of label
    for (vector<int>::size_type i = 0; i != runLabels.size(); i++)
        runLabels[i] = labelFlag[runLabels[i] - 1];

	return equaList.size();
}



void Bwlabel::getConnectedZone(const int NumberOfRuns,const vector<int>& stRun,const vector<int>& enRun, const vector<int>& rowRun,const vector<int>& runLabels){
	
	//cout << "getConnectedZone" << endl;
	int start,end,row,label;

	int labelNum = *max_element(runLabels.cbegin(), runLabels.cend());

	for(int i=0;i < NumberOfRuns;i++){
		row = rowRun[i];
		start = stRun[i];
		end = enRun[i];
		label = runLabels[i] * 200.0/labelNum;

		for(int j = start;j <= end;j++)
			mat_[row][j] = label;
	}
}
