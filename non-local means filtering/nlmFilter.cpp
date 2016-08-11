#include "nlmFilter.h"																	 

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

const string nlmFilter::nlmTypeList[4] = { "1-Vectorial", "2-Adaptive", "3-Plugin", "4-Pearson" };

nlmFilter::nlmFilter(const float sigma) : sigma_(sigma){

	cout << "call nlmFilter " << endl;

	if (sigma_ > 0.0f && sigma_ <= 15.0f) {
		fpatch_ = 1;
		rblock_ = 10;
		kfilter_ = 0.4f;
	}
	else if (sigma_ > 15.0f && sigma_ <= 30.0f) {
		fpatch_ = 2;
		rblock_ = 10;
		kfilter_ = 0.4f;
	}
	else if (sigma_ > 30.0f && sigma_ <= 45.0f) {
		fpatch_ = 3;
		rblock_ = 17;
		kfilter_ = 0.35f;

	}
	else if (sigma_ > 45.0f && sigma_ <= 75.0f) {
		fpatch_ = 4;
		rblock_ = 17;
		kfilter_ = 0.35f;

	}
	else if (sigma_ <= 100.0f) {
		fpatch_ = 5;
		rblock_ = 17;
		kfilter_ = 0.30f;
	}
	else {
		cout << "error: sigma ( 0.0~100.0 )." << endl;
		exit(-1);
	}
}

bool nlmFilter::filter(vector<int>& image, const int width, const int height) const {

	cout << "call filter " << endl;
	if (width*height != image.size())	 {
		cout << "error: image size.";
		exit(-1);
	}

	int imageSize = width*height;

	vector<float>  inBGR(imageSize, 0.0f);							
	vector<float>  outBGR(imageSize, 0.0f);

	for (int i = 0; i < imageSize; ++i)	 
		inBGR[i] = static_cast<float>(image[i]);   //transform int to float.

	if ( Vectorial == nlmType_ || Adaptive == nlmType_ ||Pearson == nlmType_)
	    traverseXY(width, height, inBGR, inBGR, outBGR);
	else if (Plugin == nlmType_) {
		vector<float>  midBGR(imageSize, 0.0f);
		traverseXY(width, height, inBGR, inBGR,  midBGR);
		traverseXY(width, height, inBGR, midBGR, outBGR);
	}
	else cout << "error: nlmfilter type. " << endl;

	for (int i = 0; i < imageSize; ++i)
		image[i] = static_cast<int>(outBGR[i]);   //transform float to int.

	return true;

}

void nlmFilter::traverseXY(const int width, const int height, const vector<float>& inBGR, const vector<float>& midBGR, vector<float>& outBGR) const {
	cout << "call traverseXY " << endl;
	const int patch_ = 2 * fpatch_ + 1;
	const int patchSize_ = patch_ * patch_;

	const int wxh = width*height;

	// denoised patch centered at a certain pixel with size of patchSize_. 
	vector<float> denoisedPatch(patchSize_, 0.0f);

	vector<int> count(wxh, 0);	 //store the count of each x,y

    //-----------------------------------------
	//construct exp(-x) table.
	//const unsigned int expLutLength = expLutMax*expLutPrecision;
	//vector<float> expLut(expLutLength, 0.0f);

	//for (int i = 0; i != expLutLength; ++i)
	//	expLut[i] = expf( - static_cast<float>(i) / static_cast<float>(expLutPrecision) );
	//-----------------------------------------

	//construct SSI matrix( summed squared Image)
    // vector<vector<float>> SSI(height,vector<float> (width, 0.0f));
	//createSSI(width, height,inBGR, SSI);


	for (int y = 0; y < height;  y += interval_)
	for (int x = 0; x < width;   x += interval_) {

		// reduce the size of the comparison window when nearing the boundary	
		// determine the minor one amoung the distance of (x,y) to the nearest boundary
		const int fpatch = min(fpatch_, min(width - 1 - x, min(height - 1 - y, min(x, y))));
		const int patchSize = (2 * fpatch + 1)*(2 * fpatch + 1);

		// research zone depending on the boundary and the size of the window
		const int imin = max(x - rblock_, fpatch);
		const int jmin = max(y - rblock_, fpatch);
		const int imax = min(x + rblock_, width - 1 - fpatch);
		const int jmax = min(y + rblock_, height - 1 - fpatch);

		// maximum of weights. Used for reference patch
		float maxWeight = 0.0f;
		// sum of weights
		float totalWeight = 0.0f;

		// usd to test wheather all different patched are ignored.
		int flag = 0;

		for (int j = jmin; j <= jmax; ++j)
		for (int i = imin; i <= imax; ++i)
		if (i != x || j != y) {
			//calculate the distance of pathc centered at q and p.
			float dist2 = distancePQ(width, height, inBGR, midBGR, x, y, i, j, fpatch);
			// float dist2 = distancePQ(width, height, inBGR, x, y, i, j, fpatch,SSI);	

			//skip current patch comparsion.
			if ( isAdaptive_ && dist2 < 0.0f) continue;
			flag = 1;	   //if skip tempCount won't ++.

			
			if (Vectorial == nlmType_)	 {	 
				dist2 /= static_cast<float>(patchSize);	  //normalize.	patchSize_ or patchSize	 ?
				// d^2 - 2 * sigma^2  
				dist2 = max(dist2 - 2.0f * sigma_*sigma_, 0.0f) / (kfilter_*sigma_) / (kfilter_*sigma_);
			    // dist2 = dist2 / (kfilter_*sigma_) / (kfilter_*sigma_);
			}
			else if (Adaptive == nlmType_ || Plugin == nlmType_)  {
				//dist2 -= 2.0f*sigma_*sigma_;
				dist2 = static_cast<float>(sqrt(dist2) )/ sigma_ - static_cast<float>( sqrt(2 * patchSize_ - 1) ) ;
				dist2 = 0.5f*dist2*dist2;
			}
			else if (Pearson == nlmType_)	  {		
				dist2 /= static_cast<float>(patchSize);	  //normalize.
				dist2 = dist2 / (kfilter_*sigma_) / (kfilter_*sigma_);
			}

			float weight = exp(-dist2);
			//float weight = findInExpLut(expLut, dist2);
			if (weight > maxWeight) maxWeight = weight;
			totalWeight += weight;

			for (int u = -fpatch; u <= fpatch; ++u)
			for (int v = -fpatch; v <= fpatch; ++v)	{
				//	find the first pixel index of each line in denoisedPatch .
				const int indexInDenoised = (fpatch_ + u) * patch_ + fpatch_ + v;
				//	find the first pixel of each line index in image. 
				const int indexInImage = (j + u)*width + i + v;
				denoisedPatch[indexInDenoised] += weight * midBGR[indexInImage];
			}
		} // i,j loop end

		if ( 0 == flag) // can't find any similar patch 
		    maxWeight = 1.0f;
		
		//patch centered at x,y, use maximum weight.
		for (int u = -fpatch; u <= fpatch; ++u)
		for (int v = -fpatch; v <= fpatch; ++v)	 {
			const int indexInDenoised = (fpatch_ + u) * patch_ + fpatch_ + v;
			const int indexInImage = (y + u)*width + x + v;
			denoisedPatch[indexInDenoised] += maxWeight * midBGR[indexInImage];
		}

		totalWeight += maxWeight;

		// normalize average value when totalweight is not near zero.
		if (totalWeight > weightThreshold) {
			for (int u = -fpatch; u <= fpatch; ++u)
			for (int v = -fpatch; v <= fpatch; ++v)	{
				const int indexInDenoised = (fpatch_ + u) * patch_ + fpatch_ + v;
				const int indexInImage = (y + u)*width + x + v;
				count[indexInImage]++;
				outBGR[indexInImage] += denoisedPatch[indexInDenoised] / totalWeight;
			}
		}

		//for every x,y, clear denoisedPatch to 0.0f;
		denoisedPatch.assign(patchSize_, 0.0f);

	} // y,x loop end

	for (int i = 0; i < wxh; ++i) {
		if (count[i] > 0) outBGR[i] /= static_cast<float>(count[i]);
		else   outBGR[i] = inBGR[i];
	}
}


float nlmFilter::distancePQ(const int width, const int height, const vector<float>& inBGR, const vector<float>& midBGR, const int x, const int y, const int i, const int j, const int fpatch) const{
	
	if ( isAdaptive_  ){
		float mxy = 0.0f, mij = 0.0f;

		for (int u = -fpatch; u <= fpatch; ++u)
		for (int v = -fpatch; v <= fpatch; ++v)	 {
			const int indexInImage1 = (y + u)*width + x + v;
			const int indexInImage2 = (j + u)*width + i + v;
			mxy += inBGR[indexInImage1];
			mij += midBGR[indexInImage2];
		}

		mxy /= (2 * fpatch_ + 1) *(2 * fpatch_ + 1); //ÓÃ fpatch_»¹ÊÇfpatch£¿
		mij /= (2 * fpatch_ + 1) *(2 * fpatch_ + 1);

		//if ((mxy - mij) > (lim1*sigma_ / (2 * fpatch_ + 1)))	
		if (min(mxy, mij) / max(mxy, mij) < lim1)	return -1.0f;

		float varxy = 0.0f, varij = 0.0f;
		for (int u = -fpatch; u <= fpatch; ++u)
		for (int v = -fpatch; v <= fpatch; ++v)	 {
			const int indexInImage1 = (y + u)*width + x + v;
			const int indexInImage2 = (j + u)*width + i + v;
			varxy += (inBGR[indexInImage1] - mxy) * (inBGR[indexInImage1] - mxy);
			varij += (midBGR[indexInImage2] - mij) * (midBGR[indexInImage2] - mij);
		}

		varxy /= (2 * fpatch_ + 1) *(2 * fpatch_ + 1); 
		varij /= (2 * fpatch_ + 1) *(2 * fpatch_ + 1);

		//if ((max(varxy, varij) / min(varxy, varij)) > lim2)
		if (min(varxy, varij) / max(varxy, varij) < lim2)  return -1.0f;
	}

	float totalDist, dist;

	if (  Vectorial == nlmType_ || Adaptive == nlmType_ || Plugin == nlmType_) {	
		totalDist = 0.0f;
		for (int u = -fpatch; u <= fpatch; ++u)
		for (int v = -fpatch; v <= fpatch; ++v)	 {
			const int indexInImage1 = (y + u)*width + x + v;
			const int indexInImage2 = (j + u)*width + i + v;
			dist = inBGR[indexInImage1] - midBGR[indexInImage2];
			dist *= dist;
			totalDist += dist;
		}
	}
	else if ( Pearson == nlmType_)	 {
		totalDist = 0.0f;
		for (int u = -fpatch; u <= fpatch; ++u)
		for (int v = -fpatch; v <= fpatch; ++v)	 {
			const int indexInImage1 = (y + u)*width + x + v;
			const int indexInImage2 = (j + u)*width + i + v;
			dist = inBGR[indexInImage1] - midBGR[indexInImage2];
			dist = dist * dist / midBGR[indexInImage2];
	
			totalDist += dist;
		}
	}
	else cout << "error: non-local means filter type. " << endl;

	return totalDist;
}



float nlmFilter::findInExpLut(const vector<float>& expLut, const float dist) const{


	if (dist > expLutMax)  return 0.0f;

	const int i = int(dist*expLutPrecision);

	float expDist = expLut[i] + (expLut[i + 1] - expLut[i])*(dist*expLutPrecision - i);

	return expDist;

}





















//float nlmFilter::distancePQ(const int width, const int height, const vector<float>& inBGR, const vector<float>& midBGR, const int x, const int y, const int i, const int j, const int fpatch) const{
//	//cout << "call distancePQ " << endl;
//	float totalDist = 0.0f, dist;
//	//	find the first pixel index in image. 
//	for (int u = -fpatch; u <= fpatch; ++u)
//	for (int v = -fpatch; v <= fpatch; ++v)	 {
//		const int indexInImage1 = (y + u)*width + x + v;
//		const int indexInImage2 = (j + u)*width + i + v;
//		dist = inBGR[indexInImage1] - midBGR[indexInImage2];
//		dist *= dist;
//		totalDist += dist;
//	}
//
//	return totalDist;
//}

//void nlmFilter::createSSI(const int width, const int height, const vector<float>& inBGR, vector<vector<float>>& SSI) const {
//
//	SSI[0][0] = inBGR[0] * inBGR[0];
//	//first row
//	for (int i = 1; i < width; ++i)
//		SSI[0][i] = SSI[0][i - 1] + inBGR[i] * inBGR[i];
//	//first coloum
//	for (int j = 1; j < height; ++j)
//		SSI[j*width][0] = SSI[(j - 1)*width][0] + inBGR[j*width] * inBGR[j*width];
//
//	for (int j = 1; j < height; ++j)
//	for (int i = 1; i < width; ++i)
//		SSI[j][i] = SSI[j-1][i] + SSI[j][ i - 1] \
//		- SSI[j - 1][i - 1] \
//		+ inBGR[j*width + i] * inBGR[j*width + i];
//}
//
//
//
//float nlmFilter::distancePQ(const int width, const int height, const vector<float>& inBGR, const int x, const int y, const int i, const int j, const int fpatch, const vector<vector<float>>& SSI) const{
//	//cout << "call distancePQ " << endl;
//	float totalDist = 0.0f, dist;
//	//	find the first pixel index in image. 
//
//	for (int u = -fpatch; u <= fpatch; ++u)
//	for (int v = -fpatch; v <= fpatch; ++v)	 {
//		int index1 = (y + u)*width + x + v;
//		int index2 = (j + u)*width + i + v;
//		dist = -2.0f * inBGR[index1] * inBGR[index2];
//		totalDist += dist;
//	}
//
//	// 1--------2
//	// ----------
//	// ----------
//	// 3--------4
//	int xx[4],yy[4];
////	float sum2;
//
//	// patch centered at x,y
//	yy[0] = y - fpatch - 1;
//	xx[0] = x - fpatch - 1;
//	yy[1] = y - fpatch - 1;
//	xx[1] = x + fpatch + 1;
//
//	yy[2] = y + fpatch;
//	xx[2] = x - fpatch - 1;
//	yy[3] = y - fpatch;
//	xx[3] = x + fpatch ;
//
//	for (int i = 0; i < 4; ++i) {
//		if ( xx[i] < 0 || yy[i]) xx[i] = 0;
//	}
//	
//
//	//sum2 = SSI[index1] + SSI[index4] - SSI[index2] - SSI[index3];
//
//	//totalDist += sum2;
//
//	//// patch centered at i,j
//	//index1 = (j - fpatch - 1)*width + i - fpatch - 1;
//	//index2 = (j - fpatch - 1)*width + i + fpatch;
//	//index3 = (j + fpatch)*width + i - fpatch - 1;
//	//index4 = (j + fpatch)*width + i + fpatch;
//
//	//sum2 = SSI[index1] + SSI[index4] - SSI[index2] - SSI[index3];
//
//	//totalDist += sum2;
//
//	return totalDist;
//}
//
//

