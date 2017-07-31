// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include "swiftMatchedVectCount.h"
#include "DistProcesdDesciptr.h"
#include<cmath>


//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;



vector<SiftDescriptor> createSiftDescriptor(string inputFile){
	CImg<double> input_image(inputFile.c_str());
	// convert image to grayscale
	CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
	vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
	return descriptors;
	}

vector<DistProcesdDesciptr *> OneVsAllDescriptorMatchVctor;
vector<DistProcesdDesciptr *> MatchedSwiftVectorsOfSingleImage;
vector<swiftMatchedVectCount *> FinalResultOfBestMatchImages;
vector<vector<SiftDescriptor> > AllImageStoredDescriptors;
vector<vector<DistProcesdDesciptr *> > FinalStoreOfAllMatchesAllImages;


vector<SiftDescriptor> generateSiftDescriptors(string inputFile){

        CImg<double> input_image(inputFile.c_str());
        // convert image to grayscale
        CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
        vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
        return descriptors;

}

float swiftMatchingThreshold = 0.8;

float CalculateEucledianBetweenVetcors(SiftDescriptor siftVectorImage_1,SiftDescriptor siftVectorImage_2){
	float  eucledianDistanceValue=0;
	//cout<< "calculating the eucledian distance of 2 vectors" << endl;
	for(int l=0; l<128; l++){
		eucledianDistanceValue = eucledianDistanceValue +pow( siftVectorImage_1.descriptor[l] - siftVectorImage_2.descriptor[l],2);
	}

	return sqrt(eucledianDistanceValue);
}



void  MatchGivenTwoImageDescriptors(vector<SiftDescriptor> imageDescriptor_1,vector<SiftDescriptor> imageDescriptor_2,string image1, string image2){

	for(unsigned int i=0; i<imageDescriptor_1.size(); i++)
          {
		for(unsigned int k=0; k<imageDescriptor_2.size(); k++){


 /*          		cout << "Descriptor #" << i << ": x=" << descriptors[i].col << " y=" << descriptors[i].row << " descriptor=(";
            		for(int l=0; l<128; l++){
              			cout << descriptors[i].descriptor[l] << "," ;}
            		cout << ")" << endl;

            		for(int j=0; j<5; j++){
              		for(int k=0; k<5; k++){
               			 if(j==2 || k==2){
                 		 for(int p=0; p<3; p++){

                    			input_image(descriptors[i].col+k-1, descriptors[i].row+j-1, 0, p)=0;}}}}
*/

		float  matchedEucledianValue = CalculateEucledianBetweenVetcors(imageDescriptor_1[i],imageDescriptor_2[k]);

		//Now lets insert all the values in to the class; so lets create the object and insert the data
   		//int *f;
		DistProcesdDesciptr *f1;        
        	f1 = new DistProcesdDesciptr;
        	f1->setImageName1(image1);
		f1->setImageName2(image2);
        	f1->setCoordImage1(imageDescriptor_1[i].col,imageDescriptor_1[i].row);
		f1->setCoordImage2(imageDescriptor_2[k].col,imageDescriptor_2[k].row);
		f1->setEucledianDistanceValue(matchedEucledianValue); 
	//	f1->printDistProcesdDesciptr();
		

		OneVsAllDescriptorMatchVctor.push_back(f1);

/*
		for (vector<DistProcesdDesciptr *> ::iterator ivc = OneVsAllDescriptorMatchVctor.begin(); ivc != OneVsAllDescriptorMatchVctor.end(); ++ivc) {
			cout << "coming from the pointer vector" << endl;     			
			(*ivc)->printDistProcesdDesciptr();
			}

*/



		}

	// Now we should sort by eucledian distance
	std::sort( OneVsAllDescriptorMatchVctor.begin(),  OneVsAllDescriptorMatchVctor.end(), DistProcesdDesciptr::compareMyDataPredicate);

	/* 
 	        cout << "Sorting by Eucledian Value" << endl;
                for (vector<DistProcesdDesciptr *> ::iterator ivc = OneVsAllDescriptorMatchVctor.begin(); ivc != OneVsAllDescriptorMatchVctor.end(); ++ivc) {
                                             
                        (*ivc)->printDistProcesdDesciptr();
                        }
	*/

	//OneVsAllDescriptorMatchVctor[0]->printDistProcesdDesciptr();
	//cout << *(OneVsAllDescriptorMatchVctor[0]).getEucledianDistanceValue();
	//cout <<  OneVsAllDescriptorMatchVctor[0]->getEucledianDistanceValue();

	if  ((OneVsAllDescriptorMatchVctor[0]->getEucledianDistanceValue() /  OneVsAllDescriptorMatchVctor[1]->getEucledianDistanceValue()) <= swiftMatchingThreshold){
		OneVsAllDescriptorMatchVctor[0]->setIsSwiftVectorsMatched();		
		MatchedSwiftVectorsOfSingleImage.push_back(OneVsAllDescriptorMatchVctor[0]);
		
		OneVsAllDescriptorMatchVctor.clear();
		cout<<"This is a matched swift vector, added into the new vector- MatchedSwiftVectorsOfSingleImage"<<endl;
		MatchedSwiftVectorsOfSingleImage.back()->printDistProcesdDesciptr();
		
		}
	OneVsAllDescriptorMatchVctor.clear();


    	}

	//return  MatchedSwiftVectorsOfSingleImage;

}



void  compareImages(string image1, string image2,int InputImageIndex){
	
	// calculate the descriptors for the images
	vector<SiftDescriptor> imageDescriptor_1 = createSiftDescriptor(image1);
	vector<SiftDescriptor> imageDescriptor_2 = createSiftDescriptor(image2);
	cout << "created the descriptors"<<endl;
	//AllImageStoredDescriptors.push_back(imageDescriptor_1);
	//AllImageStoredDescriptors.push_back(imageDescriptor_2);
	cout << "pushed the descriptors" <<endl; 
	cout << "Going to Match Sift Image Descriptors"<< endl;
	MatchGivenTwoImageDescriptors(imageDescriptor_1,imageDescriptor_2,image1,image2);
	//return MatchedSwiftVectorsOfSingleImage;
}




void drawlinesMatching(CImg<double> image1,vector<DistProcesdDesciptr *> MatchedSwiftVectorsOfSingleImage,int widthOfImage1){

// loop through all the matched points and draw lines

                for (vector<DistProcesdDesciptr *> ::iterator ivc = MatchedSwiftVectorsOfSingleImage.begin(); ivc != MatchedSwiftVectorsOfSingleImage.end(); ++ivc) {
                                             
                        (*ivc)->printDistProcesdDesciptr();
     			const unsigned char color[] = { 255,128,64 };
		//	image1.draw_line((int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),(int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),color); 
                  image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),color);
   } 
		image1.save("fileTest.png");



                for (vector<DistProcesdDesciptr *> ::iterator ivc = MatchedSwiftVectorsOfSingleImage.begin(); ivc != MatchedSwiftVectorsOfSingleImage.end(); ++ivc) {

                        (*ivc)->printDistProcesdDesciptr();
                        const unsigned char color[] = { 255,128,64 };
   			 image1.draw_line((int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),(int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),color); 
                       //  image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),color);
                                           }
                         image1.save("fileTest2.png");
                

                for (vector<DistProcesdDesciptr *> ::iterator ivc = MatchedSwiftVectorsOfSingleImage.begin(); ivc != MatchedSwiftVectorsOfSingleImage.end(); ++ivc) {

                        (*ivc)->printDistProcesdDesciptr();
                        const unsigned char color[] = { 255,128,64 };
                        image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col(), (int)(*ivc)->getCoordImage2_row()+widthOfImage1,color);
                       //  image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),color);
                                                                    }
                          image1.save("fileTest3.png");


                for (vector<DistProcesdDesciptr *> ::iterator ivc = MatchedSwiftVectorsOfSingleImage.begin(); ivc != MatchedSwiftVectorsOfSingleImage.end(); ++ivc) {

                        (*ivc)->printDistProcesdDesciptr();
                        const unsigned char color[] = { 255,128,64 };
                        image1.draw_line((int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage1_col(), (int)(*ivc)->getCoordImage2_row()+widthOfImage1,(int) (*ivc)->getCoordImage2_col(),color);
                       //  image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),color);
                                                         }
                       image1.save("fileTest4.png");

}



void  createTheDescriptorJoinViz(string image1path, string image2path, vector<DistProcesdDesciptr *> MatchedSwiftVectorsOfSingleImage){
	int widthOfImage1 = 0;
	CImg<double> image1;
	image1.load(image1path.c_str());
    	CImg<double> image2;
        image2.load(image2path.c_str());
	widthOfImage1 = image1._width;
	cout << "image width before appending is " << image1._width << endl;
	image1.append(image2);
 	image1.normalize(0, 255);
	image1.save("concatImage.png");

	cout << "image width after appending is " << image1._width << endl;
	//image1.save("fileTest.png");
	drawlinesMatching(image1,MatchedSwiftVectorsOfSingleImage,widthOfImage1);
}


void Assignment1Part1Calulcation(){


int counter=0;

for(int i=2; i<argc-1; i=i+1){
         MatchedSwiftVectorsOfSingleImage.clear();
         compareImages(argv[2],argv[i+1],i);

// counting the total matches and inserting into objects
//          if(MatchedSwiftVectorsOfSingleImage.size()>0){
//                          swiftMatchedVectCount *s1;
//                                          s1 = new swiftMatchedVectCount;
//                                                          s1->setImageName1(argv[2]);
//                                                                          s1->setImageName2(argv[i+1]);
//                                                                                          s1->setTotalMatchesBetweenImage1and2( MatchedSwiftVectorsOfSingleImage.size());
//
//                                                                                                          FinalResultOfBestMatchImages.push_back(s1);
//                                                                                                                          FinalStoreOfAllMatchesAllImages.push_back(MatchedSwiftVectorsOfSingleImage);
//                                                                                                                                          createTheDescriptorJoinViz(argv[2],argv[i+1],MatchedSwiftVectorsOfSingleImage);
//                                                                                                                                                  }
//                                                                                                                                                  counter = counter+1;
//                                                                                                                                                  cout << "Finished Executing ::  "<<counter << "  ::  files over the given query image"<< endl;
//                                                                                                                                                  cout << "The last processed image is :  "<< argv[i+1] << endl;
//                                                                                                                                                  cout << "total argc count   " << argc << endl;
//                                                                                                                                                  }
//
//
//                                                                                                                                                  // sort the resultant image results.
//                                                                                                                                                  // Now we should sort by total matches
//                                                                                                                                                           std::sort( FinalResultOfBestMatchImages.begin(),  FinalResultOfBestMatchImages.end(), swiftMatchedVectCount::compareMyDataPredicate);
//                                                                                                                                                                   int counterOfBestMatches = 0;
//                                                                                                                                                                                   for (vector<swiftMatchedVectCount *> ::iterator ivc = FinalResultOfBestMatchImages.begin(); ivc != FinalResultOfBestMatchImages.end(); ++ivc) {
//                                                                                                                                                                                                           counterOfBestMatches = counterOfBestMatches+1;
//                                                                                                                                                                                                                                   cout << "Match Number :: " << counterOfBestMatches << endl;
//                                                                                                                                                                                                                                                           (*ivc)->printswiftMatchedVectCount();
//                                                                                                                                                                                                                                                                                   }
//                                                              



}


int main(int argc, char **argv)
{
  try {

    if(argc < 2)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    a2 part_id ..." << endl;
	return -1;
      }

    string part = argv[1];
    string inputFile = argv[2];

if(part == "part1")
      {
	// This is just a bit of sample code to get you started, to
	// show how to use the SIFT library.


/*
	CImg<double> input_image(inputFile.c_str());

	// convert image to grayscale
	CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
		vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);

	for(int i=0; i<descriptors.size(); i++)
	  {
	    cout << "Descriptor #" << i << ": x=" << descriptors[i].col << " y=" << descriptors[i].row << " descriptor=(";
	    for(int l=0; l<128; l++)
	      cout << descriptors[i].descriptor[l] << "," ;
	    cout << ")" << endl;

	    for(int j=0; j<5; j++)
	      for(int k=0; k<5; k++)
		if(j==2 || k==2)
		  for(int p=0; p<3; p++)

		    input_image(descriptors[i].col+k-1, descriptors[i].row+j-1, 0, p)=0;

	  }

	input_image.get_normalize(0,255).save("sift.png");

*/


// For precision claculation, the input should be changed




int counter=0;

for(int i=2; i<argc-1; i=i+1){
	 MatchedSwiftVectorsOfSingleImage.clear();
	 compareImages(argv[2],argv[i+1],i);
	
// counting the total matches and inserting into objects
         if(MatchedSwiftVectorsOfSingleImage.size()>0){ 
		swiftMatchedVectCount *s1;
        	s1 = new swiftMatchedVectCount;
                s1->setImageName1(argv[2]);
                s1->setImageName2(argv[i+1]);
                s1->setTotalMatchesBetweenImage1and2( MatchedSwiftVectorsOfSingleImage.size());
	
		FinalResultOfBestMatchImages.push_back(s1);
		FinalStoreOfAllMatchesAllImages.push_back(MatchedSwiftVectorsOfSingleImage);
		createTheDescriptorJoinViz(argv[2],argv[i+1],MatchedSwiftVectorsOfSingleImage);
	}
counter = counter+1;
cout << "Finished Executing ::  "<<counter << "  ::  files over the given query image"<< endl;
cout << "The last processed image is :  "<< argv[i+1] << endl;
cout << "total argc count   " << argc << endl;
}


// sort the resultant image results.
// Now we should sort by total matches
         std::sort( FinalResultOfBestMatchImages.begin(),  FinalResultOfBestMatchImages.end(), swiftMatchedVectCount::compareMyDataPredicate);
	int counterOfBestMatches = 0;
                for (vector<swiftMatchedVectCount *> ::iterator ivc = FinalResultOfBestMatchImages.begin(); ivc != FinalResultOfBestMatchImages.end(); ++ivc) {
			counterOfBestMatches = counterOfBestMatches+1;                         
			cout << "Match Number :: " << counterOfBestMatches << endl;                 
                        (*ivc)->printswiftMatchedVectCount();
                        }


// caluclate the precisison 



      }





    else if(part == "part2")
      {
	// do something here!
      }
    else if(part == "part3")
      {
	// do something here!
      }
    else
      throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








