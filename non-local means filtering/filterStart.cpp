
#include "filterStart.h"

void filterStart(const string& workDir, const string& imageName,const int index){

	
	cout << "----------------------------------------" << endl;

	cout << imageName << endl;

	Bitmap bmp1(workDir + "\\Image\\" + imageName, imageType::GRAY);

	int width, height, channels;
	vector<int> image;
	bmp1.getVector(image, width, height, channels);

	//cout << endl;
	//cout << width << '\t' << height << '\t' << channels << endl;

	vector<int> image1(image);

	//float sigma = 20.0f;
	nlmFilter nlmfilter(20.0f);

	nlmfilter.nlmTypeSetting(nlmType(index));

	nlmfilter.filter(image, width, height);

	Bitmap bmp2(image, width, height, channels);

	//cout << endl;
	//cout << width << '\t' << height << '\t' << channels << endl;
	string filterPath = workDir + "\\" + nlmfilter.getnlmType(index) + "\\";

	//cout << filterPath + imageName;
	bmp2.write(filterPath + imageName);

	//difference
	for (int j = 0; j != image.size(); j++)
		image1[j] -= image[j];
	Bitmap bmp3(image1, width, height, channels);

	cout << endl;
	cout << width << '\t' << height << '\t' << channels << endl;

	bmp3.write(filterPath + "\\Differ\\" + imageName);

	cout << "----------------------------------------" << endl;
}

