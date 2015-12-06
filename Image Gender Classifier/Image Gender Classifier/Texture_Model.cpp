#include "Texture_Model.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/legacy/legacy.hpp"


Mat Texture_Model::VecToMat(const vector<Point2f>& vec){
    
	Mat dst=Mat(vec.size()*2, 1, CV_32F); 
	for (int i=0; i<vec.size(); i++){
		dst.FL(2*i, 0)=vec[i].x; 
		dst.FL(2*i+1, 0)=vec[i].y; 
	}
	return dst;
}

vector<Point2f> Texture_Model::MatToVec (const Mat& src){
	vector<Point2f> pts; 
	for (int i=0; i<src.rows; i+=2){
		pts.push_back(Point2f(src.FL(i), src.FL(i+1))); 
	}
	return pts; 
}

void Texture_Model::Triangulate(const vector<Point2f>&points, Subdiv2D& subdiv){
	CvRect rect={0, 0, 480, 640}; //width, height
	subdiv=Subdiv2D(rect);    //planar subdivision object
	subdiv.insert(points);
		//inserts points while adjusting the triangulation
	}

void Texture_Model::Warp( Mat& img, const Subdiv2D& subdiv, const vector<Point2f>& img_shape){
	Mat final_warp=Mat::zeros(img.rows, img.cols, img.type()); 
	vector<Vec6f> triangleList; 
    subdiv.getTriangleList(triangleList);       
	CvPoint CVref_pt[3]; //and a CvPoint array for the mean points, to pass to CvFillConvexPoly
	Point2f pt[3]; 
	Point2f ref_pt[3]; 
	bool draw; 
	for (int i = 0; i < triangleList.size(); i++ )
    {   //get next triangle in image to be warped
        Vec6f t = triangleList[i];
        pt[0] = Point((t[0]), (t[1]));
        pt[1] = Point((t[2]), (t[3]));
        pt[2] = Point((t[4]), (t[5]));

		draw=true;

      for(int i=0; i<3; i++){ //check that point is not at infinity
         if(pt[i].x>=img.cols || pt[i].y>=img.rows || pt[i].x<=0 || pt[i].y<=0)
            draw=false;
      }

	  if (draw){
	  for (int j=0; j<3; j++) //for each point in the triangle
		 for (int i=0; i<img_shape.size(); i++){ //go through image points
			if (pt[j]==img_shape[i]){ // match the point with its index
				ref_pt[j]=mean_shape[i]; //find its corresponding point on the mean shape
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

//there is a faster way
vector<uchar> Texture_Model::getHullMat(Mat& img){ //returns texture matrix for pixels in hull
	int total=0; 
	vector<uchar> texture_mat;   
	for(int i=0; i<img.cols; i++){ //iterate through points in subdivision area
        for (int j=0; j<img.rows; j++){   
            double distance = pointPolygonTest(HullPts, Point2f(i,j), 0); //tests if point is inside the hull
            if(distance >=0){
				total++; 
                texture_mat.push_back(img.at<uchar>(Point2f(i,j)));  //save intensity value of this point
			}
		}
    }
	return texture_mat; 
	 
}

void Texture_Model::PhotoNorm (Mat& D) { //takes as argument a texture matrix and normalizes the data 
	//Mat M (D.rows, 1, D.type()); 
    int m=D.rows; //number of pixels
	Mat a=Mat::ones(D.cols, 1, CV_32FC1); 
	Mat M = (1/nimgs)*D*a;  //compute average texture
	M = M-(1/m)*(Mat::ones(1, M.rows, M.type())*M); //M - mean "signal"
	M=M*m/norm(M, NORM_L2); //divide by standard deviation
	Mat new_mean=Mat::zeros(M.rows, M.cols, M.type()); 
	while (norm(M, new_mean, NORM_L2)>1e-6){
		M=new_mean; 
		for (int i=0; i<D.cols; i++){
			Mat d=D.col(i)-(1/m)*Mat::ones(1, D.cols, D.type())*D.col(i); //remove mean signal
			D.col(i)=d/(M.t()*D.col(i)); //divide by proj on mean
		}
		new_mean=(1/nimgs)*D*a; 
		new_mean=new_mean-(1/m)*(Mat::ones(1, M.rows, M.type())*new_mean);
		new_mean=new_mean*m/norm(new_mean, NORM_L2); //update mean texture
	}	
}	

void Texture_Model::PCA (const Mat&D, const double var, const int kmax){
	SVD svd(D*D.t()); 
	int m = min ( nimgs , D.rows);
	float vsum=0; 
	 
	for(int i = 0; i<m; i++)
				 vsum += svd.w.FL(i); 
	
	int dim=0;
	total_var=0; 
	for (dim=0; dim<m ; dim++){
		total_var+=svd.w.FL(dim); 
		if (total_var/vsum >= var){ //if desired variance is retained break from for loop
			dim++; //dim now holds the subspace dimension
			break;
		}
    }
	eigenV=svd.u;
	svd.u(Rect(0, 0, dim, D.rows)).copyTo(texture_basis); 
}


void Texture_Model::Train(int kmax) {
	cout<<"training texture model"<<endl; 
	vector<vector<uchar>> Texture_Data(nimgs); //save matrices of texture data for PCA
	Triangulate(mean_shape, ref_subdiv);  //the triangulation is saved in ref_subdiv
	for (int i=0; i<Images.size(); i++){ //iterate through images
		Mat img = imread(Images[i].name, 0); //load grayscale
		Subdiv2D subdiv; 
		Triangulate(shape_data[i], subdiv); //triangulate instance of image
		Warp(img, subdiv, shape_data[i]); //warped image now stored in img
		Texture_Data.push_back(getHullMat(img)); //getHullMat returns texture vector for pixels inside face
	} 
	Mat D (Mat(Texture_Data[0], false)); 
	cout<<D.rows<<" "<<D.cols<<endl;
	for (int i=1; i<Texture_Data.size(); i++)
	  hconcat(D, Mat(Texture_Data[i], false), D); //now each column in D is a texture vector

	Mat Data; 
	D.convertTo(Data, CV_32FC1); //for PCA
	PhotoNorm(D); //photometric normalization
	PCA(D, 0.95, kmax); 
	param_textures=texture_basis.t()*Data; 
}

