#include "localFeatures.hpp"

int local_features::getCode(bool bin[]){ //returns one out of 10 texture codes
	
	int t=0; //transitions
	int n=0; //number of 1s

	for (int i=0; i<8; i++){
		if (bin[i]!=bin[i+1]) 
			t++; 
		if (bin[i]==1) 
			n++; 
	}
		if (t>2)
			return 9; 
		else return n; 
}

void local_features::normalize (Mat& histogram){ //normalize image's LBP histogram
	double Sum=0; 
	for (int i=0; i<10; i++)
		Sum+=histogram.at<double>(i); 
	
	if (Sum!=0) {
	for (int i=0; i<10; i++)
		histogram.at<double>(i)/=Sum; 
	}
}


//returns a reference to texture histogram. takes as input grascale image (fragment of face)
Mat local_features::
	LBP(Mat& input_image, Mat& histogram) {
	 
	CV_Assert (input_image.type() == CV_8UC1);
	
	bool lbp[8]; //8 pixel lbp neighborhood
	int move[16]={-1, 1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1, -1, 0}; //starting at 0, even indices for x moves, odd for y
	
	int i, j; 
	uchar T; 
	for (int x=1; x<input_image.rows-1; x++) //skip edge pixels
		for (int y=1; y<input_image.cols-1; y++){
			T=input_image.at<uchar>(x, y);  //get pixel value of center pixel
			for (int k=0; k<=14; k+=2){ //move to neighboring pixel
				i=move[k];
				j=move[k+1]; 
				lbp[k/2]= (input_image.at<uchar>(x+i, y+j)>=T) ? 1: 0; 
			} 
			
			histogram.at<double>(getCode(lbp), 0)++; 
		}

		normalize(histogram); 

		return histogram; 
 }