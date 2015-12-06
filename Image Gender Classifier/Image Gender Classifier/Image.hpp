#ifndef IMAGE
#define IMAGE

#include <opencv2/core/core.hpp>

using namespace cv; 
using namespace std; 



class Image {
public:
	char gender;  
	string name; 

	//constructor
	Image(string N="", char a='n'): name(N), gender(a) {}
	
}; 

#endif

