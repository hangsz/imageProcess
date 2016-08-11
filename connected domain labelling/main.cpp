
#include "labelStart.h"

int main(){

	string workDir = "c:\\users\\hang\\desktop";
	string imageName;

	for (int i = 0; i < 7; ++i)	{
		imageName = to_string(i + 1) + ".bmp";
	    labelStart(workDir, imageName);
	}
}

