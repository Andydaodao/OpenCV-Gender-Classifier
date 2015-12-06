#include "Shape_Model.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#define FL at<float>

//==============================================================================

void
Shape_Model::
train(const Mat &points,
      const int kmax, 
	  const float frac)
{
  int columns = points.cols, rows = points.rows/2;

  //align shapes with procrustes analysis
  Mat Y = procrustes(points).clone(); 
  //perform SVD on covariance matrix 
  SVD svd(Y*Y.t());
  //dimension of principal subspace
  int m = min(min(kmax, columns-1), rows-1);
  
  //integer division by zero??
  float vsum = 0.0; //sum of variances
  for (int i = 0; i < m; i++) 
	  vsum += svd.w.at<float>(i); //svd.w accesses the matrix of variances
  
  float v = 0; int k = 0; 
  //retrieve the number of components with required fraction of total variance
  for(k = 0; k < m; k++){
	  v += svd.w.at<float>(k); 
	  if(v/vsum >= frac){
		  k++; 
		  break;
	  }
  }

  if(k > m) 
	  k = m;
  
  shape_basis = svd.u(Rect(0, 0, k, 2*rows));
  parametrized_data=shape_basis.t()*points; 

}

//==============================================================================

Mat& 
Shape_Model::
procrustes(const Mat &X,
       const int itol,
       const float ftol)
{
  int N = X.cols,n = X.rows/2;

  //center each shape
  Mat P = X.clone();
  for(int i = 0; i < N; i++){
    Mat p = P.col(i);
    float mx = 0, my = 0;
    for(int j = 0; j < n; j++){
		mx += p.FL(2*j); 
		my += p.FL(2*j+1);
	}
    mx /= n; my /= n; //compute center of mass 
    for(int j = 0; j < n; j++){ //translate points
		p.FL(2*j) += 240-mx; p.FL(2*j+1) += 320-my;}
  }
  //optimise scale and rotation
  Mat C_old;
  for(int iter = 0; iter < itol; iter++){    
    mean_shape = P*Mat::ones(N,1,CV_32F)/N; 
	normalize(mean_shape, mean_shape, 4000); 
    if(iter > 0){
		if(norm(mean_shape, C_old) < ftol) 
			break;
	}
    C_old = mean_shape.clone();
   
	for(int i = 0; i < N; i++){
      Mat R = this->rot_scale_align(P.col(i), mean_shape);
      for(int j = 0; j < n; j++){
		float x = P.FL(2*j,i),y = P.FL(2*j+1,i);
		P.FL(2*j  ,i) = R.FL(0,0)*x + R.FL(0,1)*y;
		P.FL(2*j+1,i) = R.FL(1,0)*x + R.FL(1,1)*y;
      }
    }
  }

  //testing
  Mat E(640, 480, CV_8UC3); 
  Mat M(640, 480, CV_8UC3); 
  for (int i=0; i<76; i++){
	  circle (E, cvPoint(cvRound(P.col(0).FL(2*i)), cvRound(P.col(0).FL(2*i+1))), 2, Scalar(200, 0, 0));  
	  circle (M, cvPoint(cvRound(mean_shape.FL(2*i)), cvRound(mean_shape.FL(2*i+1))), 2, Scalar(200, 0, 0));
  }

  imshow("procrustes", E); 
  waitKey(0); 

  imshow("mean", M); 
  waitKey(0); 
  return P;
}
//=============================================================================
Mat
Shape_Model::
rot_scale_align(const Mat &src,
        const Mat &dst)
{
  //construct linear system
  int n = src.rows/2; float a=0,b=0,d=0;
  for(int i = 0; i < n; i++){
    d += src.FL(2*i) * src.FL(2*i  ) + src.FL(2*i+1) * src.FL(2*i+1);
    a += src.FL(2*i) * dst.FL(2*i  ) + src.FL(2*i+1) * dst.FL(2*i+1);
    b += src.FL(2*i) * dst.FL(2*i+1) - src.FL(2*i+1) * dst.FL(2*i  );
  }
  a /= d; b /= d;//solved linear system
  return (Mat_<float>(2,2) << a,-b,b,a);
}
//==============================================================================

