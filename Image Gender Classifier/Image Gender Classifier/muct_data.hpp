/*****************************************************************************
*   The class muct_data is taken from: (with minor alterations)
/*****************************************************************************
*   Non-Rigid Face Tracking
******************************************************************************
*   by Jason Saragih, 5th Dec 2012
*   http://jsaragih.org/
******************************************************************************
*   Ch6 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/
/*
  shape_model: A combined local-global 2D point distribution model
  Jason Saragih (2012)
*
*****************************************************************************/
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include "Image.hpp"

using namespace std; 
using namespace cv; 


#define FL at<float>

/*Code modified from class muct_data obtained from http://www.packtpub.com/cool-projects-with-opencv/book */
class muct_data{
public:
  Image img; 
  Mat points; 
  bool valid; 

  muct_data (string str, //constructor takes as argument string read from data file and database directory
        string muct_dir) {
    
	valid=true;
	points= Mat::zeros(2*76, 1, CV_32F); 

	img=Image(); 		
    size_t p1 = 0, p2;   //size_t is number of bytes, p1 will be used to index the beginning of the string, and p2 the delimitor ','
    
    //set image directory
    string idir = muct_dir; 
	if(idir[idir.length()-1] != '/')  idir += "/";
    idir += "jpg/"; //specify that idir is the directory of the images

    //get image name
    p2 = str.find(","); //find position of ,
    if(p2 == string::npos){ valid=false; }
    img.name = str.substr(p1,p2-p1); //assign name of image sample without the , 
 
	if( (strcmp(img.name.c_str(),"i434xe-fn") == 0) || //corrupted data 
       (img.name[1] == 'r') || img.name[5]!='a' ||
	   valid==false ){                         //ignore flipped images
		   img.name = "";   
		   valid=false; 
	}
   
	else{
	img.gender=img.name[7]; 
      img.name = idir + str.substr(p1,p2-p1) + ".jpg"; p1 = p2+1; //set the name to be the path of the image, and move p1 to the beginning of the next string
      
      //get age (index, second column)
      p2 = str.find(",",p1); //starting at p1 find the position of the next ,
      if (p2 == string::npos){ valid=false;}
      //img.age = atoi(str.substr(p1,p2-p1).c_str()); 
	  p1 = p2+1; 
      

      //get points
	  int i=0;  
      for(i= 0; i<75; i++){ //76 landmark points
		
		p2 = str.find(",", p1);     //starting at p1 find next ,
		if(p2 == string::npos){
			valid=false;
		}
		string x = str.substr(p1, p2-p1); 
		p1 = p2+1;
		
		p2 = str.find(",",p1);
		if(p2 == string::npos){
			valid=false;
		}
		string y = str.substr(p1,p2-p1); 
		p1 = p2+1;

		points.FL(2*i) = atoi(x.c_str()); 
		points.FL(2*i+1) =atoi(y.c_str());
      } //end of non-end-point saving
	 
	  //last point is different because it does not end with a ,
      p2 = str.find(",", p1); 
      if(p2 == string::npos){
		  valid=false;
	  }

      string x = str.substr(p1, p2-p1); 
	  p1 = p2+1;
      string y = str.substr(p1, str.length()-p1); //because there is no , at the end
      points.FL(2*i)= atoi(x.c_str()); 
	  points.FL(2*i+1)= atoi(y.c_str()); 
    }//end of else (valid image)
  }//end of constructor

}; //end of muct_data class
//==============================================================================