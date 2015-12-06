///*****************************************************************************
//*   Certain Functions from Shape_Model are taken from:
///*****************************************************************************
//*   Non-Rigid Face Tracking
//******************************************************************************
//*   by Jason Saragih, 5th Dec 2012
//*   http://jsaragih.org/
//******************************************************************************
//*   Ch6 of the book "Mastering OpenCV with Practical Computer Vision Projects"
//*   Copyright Packt Publishing 2012.
//*   http://www.packtpub.com/cool-projects-with-opencv/book
//*****************************************************************************/
///*
//  shape_model: A combined local-global 2D point distribution model
//  Jason Saragih (2012)
//*
//*****************************************************************************/
//
///*
//#include "Shape_Model.hpp"
//#include <iostream>
//
///*Code copied from function calc_rigid_basis obtained from http://www.packtpub.com/cool-projects-with-opencv/book */
//Mat
//Shape_Model::
//calc_rigid_basis(const Mat &P)
//{
//  //compute mean shape
//  int N = P.cols, n = P.rows/2; 
//  mean_shape = P*Mat::ones(N, 1, CV_32FC1)/N; //find mean of each feature coordinate 
//
//  //construct basis for similarity transform
//  Mat R(2*n, 4, CV_32F); 
//  for(int i = 0; i < n; i++){           //for each feature point 
//    R.FL(2*i, 0)   =  mean_shape.FL(2*i  );  //first column 
//	R.FL(2*i+1, 0) =  mean_shape.FL(2*i+1); //and  
//    R.FL(2*i, 1)   = -mean_shape.FL(2*i+1); //second column
//	R.FL(2*i+1, 1) =  mean_shape.FL(2*i  ); //rotation, and scaling, scale=mean
//   
//	R.FL(2*i, 2)   =  1.0;            //translation 
//	R.FL(2*i+1, 2) =  0.0;
//    R.FL(2*i, 3)   =  0.0;            
//	R.FL(2*i+1, 3) =  1.0;
//  }
//
//  //4 columns, 2*n rows
//  //Gram-Schmidt orthonormalization
//  for(int i = 0; i < 4; i++){
//    Mat r = R.col(i); 
//    for(int j = 0; j < i; j++){
//      Mat b = R.col(j); r -= b*(b.t()*r);
//    }
//    normalize(r, r);
//  }
//  return R;
//}
////==============================================================================
//
///*Code copied from function rot_scale_align obtained from http://www.packtpub.com/cool-projects-with-opencv/book */
//Mat
//Shape_Model::
//rot_scale_align(const Mat &src,
//        const Mat &dst)
//{
//  //construct linear system
//  int n = src.rows/2; 
//  float a=0, b=0, d=0;
//  for(int i = 0; i < n; i++){
//    d += src.FL(2*i) * src.FL(2*i  ) + src.FL(2*i+1) * src.FL(2*i+1);
//    a += src.FL(2*i) * dst.FL(2*i  ) + src.FL(2*i+1) * dst.FL(2*i+1);
//    b += src.FL(2*i) * dst.FL(2*i+1) - src.FL(2*i+1) * dst.FL(2*i  );
//  }
//  a /= d; b /= d;//solved linear system
//  return (Mat_<float>(2,2) << a, -b, b, a);
//}
////================================================================================================
//
///*Code copied from function procrustes obtained from http://www.packtpub.com/cool-projects-with-opencv/book */
//
//Mat& Shape_Model::Procrustes (
//	const Mat &X, 
//    const int itol=100,
//    const float ftol=1e-6)
//{
//   int N = X.cols,n = X.rows/2;
//
//  //remove centre of mass
//  Mat P = X.clone();
//  for(int i = 0; i < N; i++){
//    Mat p = P.col(i);
//    float mx = 0,my = 0;
//    for(int j = 0; j < n; j++){mx += p.FL(2*j); my += p.FL(2*j+1);}
//    mx /= n; my /= n;
//    for(int j = 0; j < n; j++){p.FL(2*j) -= mx; p.FL(2*j+1) -= my;}
//  }
//  //optimise scale and rotation
//  Mat C_old;
//  for(int iter = 0; iter < itol; iter++){    
//    Mat C = P*Mat::ones(N,1,CV_32F)/N; normalize(C,C);
//    if(iter > 0){
//		if(norm(C,C_old) < ftol)
//		break;
//	}
//    C_old = C.clone();
//    for(int i = 0; i < N; i++){
//      Mat R = this->rot_scale_align(P.col(i),C);
//      for(int j = 0; j < n; j++){
//    float x = P.FL(2*j,i),y = P.FL(2*j+1,i);
//    P.FL(2*j  ,i) = R.FL(0,0)*x + R.FL(0,1)*y;
//    P.FL(2*j+1,i) = R.FL(1,0)*x + R.FL(1,1)*y;
//      }
//    }
//  }
//  Mat E(640, 480, CV_8UC3); 
//  for (int i=0; i<76; i++){
//	  circle(E, Point(cvRound(P.col(0).FL(2*i)), cvRound(P.col(0).FL(2*i+2))), 2, Scalar(255, 0, 0), 1, 8); 
//  }
//  imshow("P",  E); 
//  waitKey(0); 
//  return P;
//  
//}
////================================================================================================
//
//
///*Code modified from function calc_rigid_basis obtained from http://www.packtpub.com/cool-projects-with-opencv/book */
//
//
///*void Shape_Model::train(
//	Mat& raw_data,  
//	int kmax, const float frac) {
//		cout<<"Training shape model"<<endl;
//		//cout<<"Procrustes"<<endl;
//		//procrustes(raw_data, aligned_data); 
//		//cout<<"Rigid basis"<<endl;
//		Mat R=this->calc_rigid_basis(raw_data); 
//
//		Mat D=raw_data-R*R.t()*raw_data; //remove projections on rigid transformations. 
//		//(using formula for projection matrix, but R.t()*R=I because R is orthogonal 
//		//mean_shape=D*Mat::ones(D.cols, 1, D.type()); 
//		
//		Mat E(640, 480, CV_8UC3); 
//		for (int i=0; i<76; i++)
//			circle(E, Point(cvRound(D.col(1).FL(2*i)), cvRound(D.col(1).FL(2*i+1))), 3, Scalar(255, 0, 0), 3, 8, 0);
//		imshow("D", E); 
//		waitKey(0); 
//		cout<<"SVD"<<endl;
//		SVD svd(D*D.t()); 
//		int m = min (nshapes, npts);
//
//		float vsum = 0; //total variance
//		for(int i = 0; i < m; i++) 
//			vsum += svd.w.FL(i); //.w.fl(i) accesses the i th largest variance
//		
//		int dim = 0; 
//		total_var=0; 
//			for(dim = 0; dim < m; dim++){
//				 total_var += svd.w.FL(dim); 
//				 if(total_var/vsum >= frac){ //if desired variance is retained break from for loop
//					dim++; //dim now holds the subspace dimension
//					break;
//				}
//			}
//
//		 shape_basis.create(2*npts, 4+dim, CV_32FC1); //+4dim of rigid transformations
//		 shape_basis(Rect(0, 0, 4, 2*npts))=R;
//		 shape_basis(Rect(4, 0, dim, 2*npts))=svd.u(Rect(0, 0, dim, 2*npts));
//		 parametrized_data=shape_basis.t()*raw_data; 
//}
////=====================================================================================================*/
//
//
// //vectorize points
//void Shape_Model::train(
//	Mat& X,  
//	int kmax, const float frac) {
//  
//		//Mat X = this->pts2mat(points);
//  
//		int N = X.cols, n = X.rows/2;
//
//  //align shapes 
//  Mat Y = this->Procrustes(X); 
//
//  //compute rigid transformation
//  Mat R = this->calc_rigid_basis(Y);
//
//  //compute non-rigid transformation
//  Mat P = R.t()*Y; 
//  Mat dY = Y - R*P; 
//  SVD svd(dY*dY.t());
//  int m = min(min(kmax, N-1), n-1);
//  
//  float vsum = 0; 
//  for(int i = 0; i < m; i++)
//	  vsum += svd.w.FL(i);
//  
//  float v = 0; int k = 0; 
//  for(k = 0; k < m; k++){
//	  v += svd.w.FL(k); 
//	  if(v/vsum >= frac){
//		  k++; break;
//	  }
//  }
//  if(k > m) k = m;
//  Mat D = svd.u(Rect(0, 0, k, 2*n));
//
//  //combine bases
//  shape_basis.create(2*n, 4+k, CV_32F);
//  Mat Vr = shape_basis(Rect(0,0,4,2*n)); R.copyTo(Vr);
//  Mat Vd = shape_basis(Rect(4,0,k,2*n)); D.copyTo(Vd);
//
//  parametrized_data=shape_basis.t()*X; 
//}
////==============================================================================
//
//Mat
//Shape_Model::
//pts2mat(const vector<vector<Point2f> > &points)
//{
//  int N = points.size(); assert(N > 0);
//  int n = points[0].size();
//  for(int i = 1; i < N; i++)
//	  assert(int(points[i].size()) == n); 
//  Mat X(2*n, N, CV_32F);
//  for(int i = 0; i < N; i++){
//    Mat x = X.col(i), 
//	y = Mat(points[i]).reshape(1, 2*n); 
//	y.copyTo(x);
//  }
//  return X;
//}
//
//
