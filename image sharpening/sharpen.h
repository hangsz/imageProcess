#ifndef SHARPEN_H

#define SHARPEN_H

#include <vector>


class Sharpen{
public:

	Sharpen() = default;
	Sharpen(const int **p);
	//Sharpen(const Sharpen& sharpen);
	~Sharpen() = default;
	void laplace(std::vector<int>& image, int height, int width);

private:
	int sharp[3][3];

};


#endif