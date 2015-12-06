#include "muct_data.hpp"
#include "trainingSet.hpp"
#include <fstream>
#include <time.h>


int main() { 
	
	string muct_dir="C:\\Users\\LENOVO\\Desktop\\MUCT"; 
	string data_dir="C:\\Users\\LENOVO\\Desktop\\MUCT\\muct-landmarks\\muct76-opencv.csv"; 

	ifstream csv_file (data_dir.c_str()); //connect ifstream to excel csv file
    if(!csv_file.is_open()){ 
      cerr << "Failed opening " << data_dir << " for reading!" << endl; system("Pause");
    }

	vector<Image>training; 
	vector<Image>testing; 
	Mat training_shape_data;
	Mat testing_shape_data; 
	string str; 
	getline(csv_file, str); //read the headers
	getline(csv_file, str); //read first shape
	muct_data d(str, muct_dir); 
	training_shape_data=d.points; 
	training.push_back(d.img); 
	testing_shape_data=(d.points); 
	testing.push_back(d.img); 
	srand(365765); 
	
	while(!csv_file.eof()){
		getline(csv_file, str); //read line in data file 
		muct_data d(str, muct_dir); //construct image data
		if (!d.valid) continue; 
		if (rand()%2 == 1){ 
			training.push_back(d.img);
			hconcat(training_shape_data, d.points, training_shape_data);
		}
		else { 
			testing.push_back(d.img); 
			hconcat(testing_shape_data, d.points, testing_shape_data); 
		}
	}

	Training_Set AAM(training, training_shape_data); 
	AAM.Build_AAM(); 
	AAM.TrainSVM(); 
	AAM.Accuracy(testing, testing_shape_data); 
	system("Pause"); 
	}


