#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Image.hpp"
#include <iostream>

using namespace cv; 
using namespace std; 

#define FL at<float>

class Texture_Model {
private:
	vector<vector<Point2f>> shape_data;  //for delaunay subdivision, it takes vectors<Point2f>
	vector<Point2f> mean_shape;          //also for delaunay subdivision 
	vector<Point2f> HullPts;               //holds the coordinates of the convex hull
	Subdiv2D ref_subdiv;                 //subdivision of mean shape
	vector<Image>Images;                 //list of training images
	int nimgs;                           // number of images
	int npts;                            //number of shape points

	void PhotoNorm (Mat& D);             //Photonormalizes data matrix
	void PCA (const Mat& D, const double var, const int kmax); //performs PCA on data matrix

public:
	Mat texture_basis;                    
	Mat param_textures;                  //holds parametrized texture data
	Mat mean_texture;
	Mat eigenV;                         //holds the full set of eigenvectors found by SVD
	float total_var;                    //total variance within the PCA subdpace 
	//default constructor
	Texture_Model(){             
		nimgs=0; 
		npts=0; 
	}

	//constructor
	Texture_Model(vector<Image>&I, const Mat& shapes, const Mat& mean, int N, int n): 
	  Images(I), nimgs(N), npts(n){  //initialize the basics
	
	 for (int i=0; i<shapes.cols; i++)
		shape_data.push_back(MatToVec(shapes.col(i))); //store shape data as vectors<Point2f>

	  mean_shape=MatToVec(mean);      
	  vector<Point2f>mean_points; 
	  for (int i=0; i<mean_shape.size(); i++)
		  mean_points.push_back(Point(cvRound(mean_shape[i].x), cvRound(mean_shape[i].y))); 

	  cout<<"convex hull setup"<<endl;
	  convexHull(mean_points, HullPts, CV_CLOCKWISE, 1); //mean_shape and HullPts are vector<Point2f>

	  Mat face=imread(Images[0].name); 
	  Mat IMG(face.rows, face.cols, face.type()); 

	  for (int i=0; i<mean_shape.size(); i++)
		  circle(IMG, mean_points[i], 3, Scalar(255, 0, 0), 3, 8, 0); 
	 
	  imshow("Hull", IMG); 
	  waitKey(0); 
	  //////
	  cout<<"end of Texture model constructor"<<endl;
	 }

	vector<uchar> getHullMat(Mat& img); 
	vector<Point2f> MatToVec(const Mat& src); //gets a shape data column matrix and turns it into a vector of points
	Mat VecToMat(const vector<Point2f>& vec); //gets a vector of points and turns it into a shape data matrix
	void Triangulate(const vector<Point2f>& points, Subdiv2D& subdiv);
	void Warp( Mat& img, const Subdiv2D& subdiv, const vector<Point2f>& img_shape);
	void Train(int kmax); 

}; 