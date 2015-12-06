/*#ifndef LOCAL_FEATURES
#define LOCAL_FEATURES

#include <opencv2/core/core.hpp>
using namespace cv; 
using namespace std; 


class local_features {

private:
	Mat lbp; 
	Mat gabor; 

	Mat LBP(Mat& input_image, Mat& histogram); 
	Mat calc_features(Mat& images); 
	int getCode(bool bin[]);
	void normalize (Mat& histogram); 

	
public:
	localfeatures(); 
	Mat get_lbp(); 
	Mat get_gabor(); 
};

#endif
*/