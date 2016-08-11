#include "sharpenStart.h"

void sharpenStart(std::string& workDir, std::string& imageName){

	cout << "----------------------------------------" << endl;


	Bitmap bmp1(workDir + "\\Image\\" + imageName, imageType::BINARY);

	int width, height, channels;

	vector<int> image;

	bmp1.getVector(image, width, height, channels);


	Sharpen sharpen;

	sharpen.laplace(image, height, width);


	Bitmap bmp2(image, width, height, channels);

	//cout << endl;
	//cout << width << '\t' << height << '\t' << channels << endl;

	bmp2.write(workDir + "\\Sharpen\\" + imageName);
}