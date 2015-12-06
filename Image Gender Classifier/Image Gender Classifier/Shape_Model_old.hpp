//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include "Image.hpp"
//#include <iostream>
//
//using namespace cv; 
//using namespace std; 
//
//#define FL at<float>
//
//class Shape_Model {
//private:  
//	Mat aligned_data; 
//	int nshapes; 
//	int npts; 
//	
//	Mat& Procrustes(const Mat& X, const int itol, const float ftol);  
//	Mat rot_scale_align(const Mat& src, const Mat& dst); 
//	Mat calc_rigid_basis(const Mat &X); 
//	Mat  pts2mat(const vector<vector<Point2f> > &p);  //[x1;y1;...;xn;yn] (2nx1) CV_32F 
//   //points to vectorise
//
//public:
//	Mat shape_basis;
//	Mat eigenV; 
//	Mat parametrized_data;
//	Mat mean_shape; 
//	float total_var; 
//
//	//default constructor
//	Shape_Model() { nshapes=0; npts=0; }
//	//constructor
//	Shape_Model(int N, int n): nshapes(N), npts(n) {}
//	void train(Mat& points, int kmax, const float frac=0.95); 
//	
//}; 