#include "labelStart.h"

void labelStart(std::string& workDir, std::string& imageName){

	cout << "----------------------------------------" << endl;


	Bitmap bmp1(workDir + "\\Binary\\" + imageName, imageType::BINARY);

	int width, height, channels;

	vector<int> image;

	bmp1.getVector(image, width, height, channels);

	Bwlabel  bwlabel(image, height, width);

	int labelNum = bwlabel.work(image);

	cout << "label num: " << labelNum << endl;

	cout << endl;
	cout << width << '\t' << height << '\t' << channels << endl;

	Bitmap bmp2(image, width, height, channels);

	//cout << endl;
	//cout << width << '\t' << height << '\t' << channels << endl;

	bmp2.write(workDir + "\\Label\\" + imageName);



}


