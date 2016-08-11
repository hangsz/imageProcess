
#include "sharpen.h"

Sharpen::Sharpen(const int** p){
	for (int i = 0; i < 3;++i)
	for (int j = 0; j < 3; ++j)
		sharp[i][j] = p[i][j];
}

//Sharpen::Sharpen(const Sharpen& sharpen){
//	for (int i = 0; i < 3; ++i)
//	for (int j = 0; j < 3; ++j)
//		sharp[i][j] = sharpen.sharp[i][j];
//}

void Sharpen::laplace(std::vector<int>& image, int height, int width){
	sharp[0][0] = 0; sharp[0][1] =  1; sharp[0][2] = 0;
	sharp[1][0] = 1; sharp[1][1] = -4; sharp[1][0] = 1;
	sharp[2][0] = 0; sharp[2][1] =  1; sharp[2][2] = 0;

	sharp[0][0] = 1; sharp[0][1] =  1; sharp[0][2] = 1;
	sharp[1][0] = 1; sharp[1][1] = -8; sharp[1][0] = 1;
	sharp[2][0] = 1; sharp[2][1] =  1; sharp[2][2] = 1;

	int h = height + 2;
	int w = width + 2;

	int **mat = new int*[h];

	mat[0] = new int[w]{0};
	mat[h - 1] = new int[w]{0};
	for (int i = 1; i < h-1; ++i){
		mat[i] = new int[w];
		mat[i][0] = mat[i][w - 1] = 0;
	    for (int j = 1; j < w-1; ++j) mat[i][j] = image[(i-1)*width+j-1];
	}

	for (int i = 1; i < h - 1; ++i)
	for (int j = 1; j < w - 1; ++j){
		int differ = 0;
		for (int k = 0; k < 3; ++k)
			differ += sharp[k][0] * mat[i - k + 1][j - 1] + \
			          sharp[k][1] * mat[i - k + 1][j] + \
			          sharp[k][2] * mat[i - k + 1][j + 1];

		if ( sharp[1][1]>0 ) image[(i - 1)*width + j - 1] += differ;
		else  image[(i - 1)*width + j - 1] -= differ;
	}


}