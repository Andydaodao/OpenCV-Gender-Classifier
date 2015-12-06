#ifndef TRAINING_SET
#define TRAINING_SET

#include "Texture_Model.hpp"
#include "Shape_Model.hpp"
#include "Image.hpp"

class Training_Set {
private:
	static const int max_shape_dim = 35;
	static const int max_tex_dim = 35; 
	vector<Image>images;
	int N_imgs; 
	int npts; 
	
	Mat raw_shape_data;   
	Mat phi; 
	Mat phi_s; 
	Mat phi_t; 
	Mat W; 
	Mat parametrized_data; 

	CvSVM SVM; 

public:
	int img_indx; 
	//data fields that contain bases
	Texture_Model T; 
	Shape_Model S; 
	

	//constructor:
	Training_Set(vector <Image>I, Mat& R){ 
		cout<<"constructing training set"<<endl;
		images=I; 
		raw_shape_data=R; 
		N_imgs=images.size(); 
		npts=raw_shape_data.rows/2;

	} 

	//AAM 
	void Build_AAM () {
		cout<<"constructing shape model"<<endl;
		S=Shape_Model(N_imgs, npts);   
		S.train(raw_shape_data, max_shape_dim); //Do PCA and parametrize the raw data
		cout<<"constructing texture model"<<endl;
		T=Texture_Model(images, raw_shape_data, S.mean_shape, N_imgs, npts);
		T.Train(max_tex_dim); 

		float r=T.total_var/S.total_var;  //scaling matrix W for combining features
		W=Mat::eye(76*2, 76*2, CV_32FC1)*r; 
		Mat features=W*S.parametrized_data; 
		Mat texture_vectors;
		vconcat (features, T.param_textures, features); //combined feature matrix

		//third PCA, to remove dependencies between shape and texture
		PCA pca(features, CV_PCA_DATA_AS_COL, 0.95); 
		phi=pca.eigenvectors.t(); //principal eigenvectors are returned as rows, transpose needed
		phi(Rect(0, 0, phi.cols, 2*76)).copyTo(phi_s);  //initialize shape and texture eigenvector matrices
		phi(Rect(0, 2*76, phi.cols, phi.rows-2*76)).copyTo(phi_t); //this is for backProjection
		
		parametrized_data=pca.project(features);  //project parametrized shape and texture data onto third PCA
	}

	/*
	void :Warp( Mat& img, const Subdiv2D& subdiv, const vector<Point2f>& img_shape, const vector<Point2f>& reference){
	Mat final_warp=Mat::zeros(img.rows, img.cols, img.type()); 
	vector<Vec6f> triangleList; 
    subdiv.getTriangleList(triangleList);       
	CvPoint CVref_pt[3]; //and a CvPoint array for the mean points, to pass to CvFillConvexPoly
	Point2f pt[3]; 
	Point2f ref_pt[3]; 
	bool draw; 
	for (int i = 0; i < triangleList.size(); i++ )
    {
        Vec6f t = triangleList[i];
        pt[0] = Point((t[0]), (t[1]));
        pt[1] = Point((t[2]), (t[3]));
        pt[2] = Point((t[4]), (t[5]));

		draw=true;

      for(int i=0;i<3;i++){
         if(pt[i].x>=img.cols || pt[i].y>=img.rows || pt[i].x<=0 || pt[i].y<=0)
            draw=false;
      }


	  if (draw){
	  for (int j=0; j<3; j++)
		 for (int i=0; i<img_shape.size(); i++){
			if (pt[j]==img_shape[i]){
				ref_pt[j]=reference[i]; 
				CVref_pt[j]=Point(cvRound(ref_pt[j].x), cvRound(ref_pt[j].y)); 
				break;
			}
		 }
	 
	  Mat warp_mat=getAffineTransform(pt, ref_pt); 
      Mat warp_mask=Mat::zeros(img.rows, img.cols, img.type()); 
	  cvFillConvexPoly(new IplImage(warp_mask), CVref_pt, 3, CV_RGB(255,255,255), CV_AA, 0);
	  Mat warp_dst; 
	  warpAffine(img, warp_dst, warp_mat, img.size());
	  warp_dst.copyTo(final_warp, warp_mask); 
	  }
    }
	img=final_warp; //let img point to final_warp. no need to copy, final_warp was to be destroyed anyway.
}
	
	/*Mat BackProject(Mat& parameters){
		Mat shape=S.mean_shape + S.eigenV*W.inv()*phi_s*parameters; 
		Mat texture=T.mean_texture + T.eigenV*phi_t*parameters; 
	    
		//note that this texture vector needs to be reshaped using affine warp!
		Mat img(640, 600, CV_8UC1); 
		int k=0; 
		for(int i=0; i<640; i++){ //iterate through points in subdivision area
			 for (int j=0; j<600; j++){  
				 img.at<uchar>(Point(i, j))=texture.at<uchar>(k); 
				 k++;
			 }
		}


		vector<Point2f> pts; 
		for (int i=0; i<shape.rows; i+=2)
			pts.push_back(Point2f(shape.fl(i), shape.fl(i+1))); 
		
		
	    CvRect rect={0, 0, 640, 600}; //width, height
		subdiv=Subdiv2D(rect);       //planar subdivision object
		subdiv.insert(pts);
		Warp(img, subdiv, 
}; 
*/

	void TrainSVM(){
		Mat Data=parametrized_data.t(); //each example is in a row
		Mat labels(images.size(), 1, CV_32FC1); //-1 for males, 1 for females
		for (int i=0; i<images.size(); i++){
			labels.FL(i)= (images[i].gender=='f') ? 1 : -1; 
		}
		CvSVMParams params;
		params.svm_type    = CvSVM::C_SVC;
		params.kernel_type = CvSVM::LINEAR;
		params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 200, 1e-6); 

		SVM.train(Data, labels, Mat(), Mat(), params); 
	} 

	void Accuracy(const vector<Image>& testing_images, Mat& testing_shapes){
		int correct=0; 
		int total=testing_images.size(); 
		for (int i=0; i<total; i++){
			Mat img=imread(testing_images[i].name, 0); 
			Subdiv2D subdiv; 
			vector<Point2f>points=T.MatToVec(testing_shapes.col(i)); 
			T.Triangulate(points, subdiv); 
			T.Warp(img, subdiv, points); //warp to mean shape
			Mat texture_mat(T.getHullMat(img), false); //texture vector
			Mat features; 
			hconcat(testing_shapes.col(i), texture_mat, features); 
			Mat parameters=phi.t()*features; //project onto principal subspace
			char gender=(SVM.predict(parameters)==1) ? 'f' : 'm' ; //predict gender, returns float
			if (gender==testing_images[i].gender) correct++; 
		}

		double accuracy=(double)correct/total; 
		cout<<accuracy<<endl;
	}

}; 

#endif
