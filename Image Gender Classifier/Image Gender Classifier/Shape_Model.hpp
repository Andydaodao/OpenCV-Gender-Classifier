#ifndef _FT_SHAPE_MODEL_HPP_
#define _FT_SHAPE_MODEL_HPP_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
using namespace cv;
using namespace std;
//==============================================================================
class Shape_Model{                         //2d linear shape model
public:
	Mat aligned_data; 
	int nshapes; 
	int npts; 
	
	Mat& procrustes(const Mat &X, const int itol=100, const float ftol=1e-6);  
	Mat rot_scale_align(const Mat& src, const Mat& dst); 

public:
	Mat shape_basis;
	Mat eigenV; 
	Mat parametrized_data;
	Mat mean_shape; 
	float total_var; 

	//default constructor
	Shape_Model() { nshapes=0; npts=0; }
	//constructor
	Shape_Model(int N, int n): nshapes(N), npts(n) {}
	void train(const Mat& points, const int kmax, const float frac=0.95);

};

#endif