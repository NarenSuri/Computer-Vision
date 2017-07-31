// B657 assignment 3 skeleton code, D. Crandall
//
// Compile with: "make"
//
// This skeleton code implements nearest-neighbor classification
// using just matching on raw pixel values, but on subsampled "tiny images" of
// e.g. 20x20 pixels.
//
// It defines an abstract Classifier class, so that all you have to do
// :) to write a new algorithm is to derive a new class from
// Classifier containing your algorithm-specific code
// (i.e. load_model(), train(), and classify() methods) -- see
// NearestNeighbor.h for a prototype.  So in theory, you really
// shouldn't have to modify the code below or the code in Classifier.h
// at all, besides adding an #include and updating the "if" statement
// that checks "algo" below.
//
// See assignment handout for command line and project specifications.
//
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>
#include <fstream>
#include<sstream>
#include <cstdlib>
//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

// Dataset data structure, set up so that e.g. dataset["bagel"][3] is
// filename of 4th bagel image in the dataset
typedef map<string, vector<string> > Dataset;

#include <Classifier.h>
#include <NearestNeighbor.h>
#include <SVMBasedAlgo.h>
#include <HaarFeature.h>
#include <Eigen.h>
#include <KMeans.h>
// Figure out a list of files in a given directory.
//
vector<string> files_in_directory(const string &directory, bool prepend_directory = false)
{
  vector<string> file_list;
  DIR *dir = opendir(directory.c_str());
  if(!dir)
    throw std::string("Can't find directory " + directory);
  
  struct dirent *dirent;
  while ((dirent = readdir(dir))) 
    if(dirent->d_name[0] != '.')
      file_list.push_back((prepend_directory?(directory+"/"):"")+dirent->d_name);

  closedir(dir);
  return file_list;
}

int main(int argc, char **argv)
{
  try {
	
    if(argc < 3)
      throw string("Insufficent number of arguments");

    string mode = argv[1];
    string algo = argv[2];
	int k = 10;
    cout << "a3 | main :: info : main function started "<<endl;	

    // Scan through the "train" or "test" directory (depending on the
    //  mode) and builds a data structure of the image filenames for each class.
    Dataset filenames; 
    vector<string> class_list = files_in_directory(mode);
    for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
      filenames[*c] = files_in_directory(mode + "/" + *c, true);

    cout <<"a3 | main :: info : files loaded"<<endl;
    // set up the classifier based on the requested algo
    Classifier *classifier=0;
    SVMBasedAlgo<string> *svm_obj;
	HaarFeature<string> *haar = new HaarFeature<string>();
	haar->setAlgo(algo);
	haar->setMode(mode);
	
    KMeans<string> *kmean_obj;
    Eigen<string> *eigen_obj;
    if(algo == "nn"){
      classifier = new NearestNeighbor(class_list);
	}
	else if(algo == "baseline" || algo == "deep"){
    	svm_obj = new SVMBasedAlgo<string>(algo);
	svm_obj->call_train_test(mode,filenames);
    } else if(algo == "bow"){
	kmean_obj = new KMeans<string>();
	kmean_obj->call_train_test(mode, filenames);
    } else if(algo == "eigen"){
	eigen_obj = new Eigen<string>();
	eigen_obj->call_train_test(mode, filenames);
    }
	else if (algo == "haar")
	{
		haar->computeHaar(filenames);
	}
    else
      throw std::string("unknown classifier " + algo);

  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








