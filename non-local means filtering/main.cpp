
#include "filterStart.h"

int main(){

	time_t start, begin, end;


	time(&start);

	string workDir = "c:\\users\\hang\\desktop";
	string imageName;

	for (int i = 0; i < 7; ++i)	{	 
		time(&begin);
		imageName = to_string(i + 1) + ".bmp";
		filterStart(workDir, imageName, 2);

		time(&end);
		cout << "using time:       " << difftime(end, begin) << " s." << endl;
	}

	cout << "total using time: " << difftime(end, start) / 60.0 << " min." << endl;
}



