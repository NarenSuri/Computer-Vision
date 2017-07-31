/*
How to run the program

Part 1)
       -For checking sift correspondences
	./a2 part1 img1.jpg img2.jpg

       -For finding image matches
	./a2 part2 source_img.jpg test_img1.jpg test_img2.jpg ... test_imgn.jpg
	

       -For precision checking
	./a2 part1 source_img.jpg
		it will automatically pick from the corresponding folder. No need to specify test image set. It picks from the relative folder located at the path.


Part 2)
      -For testing using RANSAC and estimating homography between just two images
	./a2 part2 img1.jpg img2.jpg
      -For repeating experiment 3 from question 1
	./a2 part2 source_img.jpg
		it will automatically pick from the corresponding folder. No need to specify test image set. It picks from the relative folder located at the path.
      -For summarizing vector features
	./a2 part2.2 source_img.jpg test_img.jpg

Part 3)
     -For given coordinate tranformation matrix 
	./a2 part3 lincoln.jpg
     -For image sequence warping
	./a2 part3 source_img.jpg test_img1.jpg test_img2.jpg test_img3.jpg... test_imgn.jpg	
	




*/



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
#include <dirent.h>
#include "ReducedVector.h"


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
vector<string> toCheckTop10NamesOfFiles;
vector<ReducedVector> ReducedDescriptorsObjects;
vector <vector <double> > RandomVector;
string strImg1,strImg2;
/*global arrays*/
double best_homograph[3][3];
vector<DistProcesdDesciptr *> bestVectInliers;

vector<SiftDescriptor> generateSiftDescriptors(string inputFile){

        CImg<double> input_image(inputFile.c_str());
        // convert image to grayscale
        CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
        vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
        return descriptors;

}

float swiftMatchingThreshold = 0.6;
int n = 10;
int w = 5;

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
*/		}

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
		//cout<<"This is a matched swift vector, added into the new vector- MatchedSwiftVectorsOfSingleImage"<<endl;
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
	//cout << "created the descriptors"<<endl;
	//AllImageStoredDescriptors.push_back(imageDescriptor_1);
	//AllImageStoredDescriptors.push_back(imageDescriptor_2);
	//cout << "pushed the descriptors" <<endl; 
	//cout << "Going to Match Sift Image Descriptors"<< endl;
	MatchGivenTwoImageDescriptors(imageDescriptor_1,imageDescriptor_2,image1,image2);
	//return MatchedSwiftVectorsOfSingleImage;
}




void drawlinesMatching(CImg<double> image1,vector<DistProcesdDesciptr *> MatchedSwiftVectorsOfSingleImage,int widthOfImage1, string filename){

// loop through all the matched points and draw lines

                for (vector<DistProcesdDesciptr *> ::iterator ivc = MatchedSwiftVectorsOfSingleImage.begin(); ivc != MatchedSwiftVectorsOfSingleImage.end(); ++ivc) {
                                             
                       // (*ivc)->printDistProcesdDesciptr();
     			const unsigned char color[] = { 255,128,64 };
		//	image1.draw_line((int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),(int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),color); 
                  image1.draw_line((int) (*ivc)->getCoordImage1_col(),(int)(*ivc)->getCoordImage1_row(),(int) (*ivc)->getCoordImage2_col()+widthOfImage1, (int)(*ivc)->getCoordImage2_row(),color);
   } 
		image1.save(filename.c_str());
}



void  createTheDescriptorJoinViz(string image1path, string image2path, vector<DistProcesdDesciptr *> MatchedSwiftVectorsOfSingleImage, string fileName){
	int widthOfImage1 = 0;
	CImg<double> image1;
	image1.load(image1path.c_str());
    	CImg<double> image2;
        image2.load(image2path.c_str());
	widthOfImage1 = image1._width;
	cout << "image width before appending is " << image1._width << endl;
	image1.append(image2);
 	image1.normalize(0, 255);
	//image1.save("concatImage.png");

	cout << "image width after appending is " << image1._width << endl;
	//image1.save("fileTest.png");
	drawlinesMatching(image1,MatchedSwiftVectorsOfSingleImage,widthOfImage1,fileName);
}


void Assignment2Part1PrecisionCalulcation(vector<string> argv, vector<string> matchImages){
                                                    
int counter=0;

	for(int i=0; i< argv.size()-1; i=i+1){
		 MatchedSwiftVectorsOfSingleImage.clear();
		//cout << "in loop" <<endl;
		if(argv[0].compare(argv[i+1])==0){
			cout<<"matched  ";cout<< argv[0];cout<<argv[i+1];continue;}
		else{
		 	compareImages(argv[0],argv[i+1],i);

			// counting the total matches and inserting into objects
		 	if(MatchedSwiftVectorsOfSingleImage.size()>0){
				swiftMatchedVectCount *s1;
				s1 = new swiftMatchedVectCount;
				s1->setImageName1(argv[0]);
				s1->setImageName2(argv[i+1]);
				s1->setTotalMatchesBetweenImage1and2( MatchedSwiftVectorsOfSingleImage.size());

				FinalResultOfBestMatchImages.push_back(s1);
				FinalStoreOfAllMatchesAllImages.push_back(MatchedSwiftVectorsOfSingleImage);
				//createTheDescriptorJoinViz(argv[0],argv[i+1],MatchedSwiftVectorsOfSingleImage);
			     }
			counter = counter+1;
			cout << "Finished Executing ::  "<<counter << "  ::  files over the given query image"<< endl;
			cout << "The last processed image is :  "<< argv[i+1] << endl;
		}
	}

			// sort the resultant image results.
			// Now we should sort by total matches
			//and
			// lets caclulate the total precision value
        std::sort( FinalResultOfBestMatchImages.begin(),  FinalResultOfBestMatchImages.end(), swiftMatchedVectCount::compareMyDataPredicate);
        int counterOfBestMatches = 0;
        int precesionMatches=0;       
	for (vector<swiftMatchedVectCount *> ::iterator ivc = FinalResultOfBestMatchImages.begin(); ivc != FinalResultOfBestMatchImages.end(); ++ivc) {
             counterOfBestMatches = counterOfBestMatches+1;
             cout << "Match Number :: " << counterOfBestMatches << endl;
	     if (counterOfBestMatches > 10){break;}
             (*ivc)->printswiftMatchedVectCount();
	     //(*ivc)->getImageName2();		
			//cout << "inside checking the while";
			for(int kl=0; kl< toCheckTop10NamesOfFiles.size(); kl++){
				//string name = (string)(*ivc)->getImageName2();
				if(((string)(*ivc)->getImageName2()).compare(toCheckTop10NamesOfFiles[kl]) == 0){
					precesionMatches = precesionMatches+1;
					//toCheckTop10NamesOfFiles.erase((string)toCheckTop10NamesOfFiles[kl]);
					//cout << "To remove" ;
					cout << "In top 10- of same category!! "<<toCheckTop10NamesOfFiles[kl];
					//cout  << " and removed ";
					//cout  <<  toCheckTop10NamesOfFiles.begin()+kl << endl;
					//toCheckTop10NamesOfFiles.erase(toCheckTop10NamesOfFiles.begin()+kl);
					//break;
				}
			} 
		
	}
	cout << "total argc count   " << argv.size() << endl;
	std::cout << "The total precision for Query :" << argv[0] << "  is  ::: " << (precesionMatches*100)/toCheckTop10NamesOfFiles.size()<< endl;
}



void Assignment2PartABCalulcation(int argc, char **argv){

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
		cout << "The swift matches are going to be mapped and saved to file  " << (string)argv[2] << "   :: "<< (string)argv[i+1] << endl;
                createTheDescriptorJoinViz(argv[2],argv[i+1],MatchedSwiftVectorsOfSingleImage,"visualizeSiftMatches.jpg");
        }
counter = counter+1;
cout << "Finished Executing ::  "<<counter << "  ::  files over the given query image"<< endl;
cout << "The last processed image is :  "<< argv[i+1] << endl;
cout << "total argc count   " << argc << endl;


// sort the resultant image results.
// Now we should sort by total matches
         std::sort( FinalResultOfBestMatchImages.begin(),  FinalResultOfBestMatchImages.end(), swiftMatchedVectCount::compareMyDataPredicate);
        int counterOfBestMatches = 0;
                for (vector<swiftMatchedVectCount *> ::iterator ivc = FinalResultOfBestMatchImages.begin(); ivc != FinalResultOfBestMatchImages.end(); ++ivc) {
                        counterOfBestMatches = counterOfBestMatches+1;
                        cout << "Match Number :: " << counterOfBestMatches << endl;
                        (*ivc)->printswiftMatchedVectCount();
                        }
	}
}


/*inverse 8x8*/
/*Reference : http://www.ccodechamp.com/c-program-to-find-inverse-of-matrix/*/
double determinant(double a[8][8], int k) {
    double s = 1, det = 0, b[8][8];
    int i, j, m, n, c;
    if (k == 1) {
        return (a[0][0]);
    }
    else {
        det = 0;
        for (c = 0; c < k; c++) {
            m = 0;
            n = 0;
            for (i = 0; i < k; i++) {
                for (j = 0; j < k; j++) {
                    b[i][j] = 0;
                    if (i != 0 && j != c) {
                        b[m][n] = a[i][j];
                        if (n < (k - 2))
                        n++;
                        else {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * (a[0][c] * determinant(b, k - 1));
            s = -1 * s;
        }
    }
    return (det);
}
/*Finding transpose of matrix*/
void transposeM(double num[8][8], double fac[8][8], int r, double** retMat) {
    int i, j;
    double b[8][8], d;
    for (i = 0; i < r; i++) {
        for (j = 0; j < r; j++) {
            b[i][j] = fac[j][i];
        }
    }
    d = determinant(num, r);
    for (i = 0; i < r; i++) {
        for (j = 0; j < r; j++) {
            retMat[i][j] = b[i][j] / d;
        }
    }
    /*printf("nnnThe inverse of matrix is : n");
    for (i = 0; i < r; i++) {
        for (j = 0; j < r; j++) {
            printf("t%f", retMat[i][j]);
        }
        printf("n");
    }
    */
}
void cofactor(double num[8][8], int f, double** retMat) {
    double b[8][8], fac[8][8];
    int p, q, m, n, i, j;
    for (q = 0; q < f; q++) {
        for (p = 0; p < f; p++) {
            m = 0;
            n = 0;
            for (i = 0; i < f; i++) {
                for (j = 0; j < f; j++) {
                    if (i != q && j != p) {
                        b[m][n] = num[i][j];
                        if (n < (f - 2))
                        n++;
                        else {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
        }
    }
    return transposeM(num, fac, f, retMat);
}
 /*inverse 3x3*/
void inverse(double homograph[3][3]) {
    double dtm = (homograph[0][0] * ((homograph[1][1] * homograph[2][2]) -
    (homograph[1][2] * homograph[2][1]))) -
    (homograph[0][1] * ((homograph[1][0] * homograph[2][2]) -
    (homograph[1][2] * homograph[2][0]))) +
    (homograph[0][2] * ((homograph[1][0] * homograph[2][1]) -
    (homograph[1][1] * homograph[2][0])));
    double tempMat[3][3];
    tempMat[0][0] =
    (homograph[1][1] * homograph[2][2]) - (homograph[1][2] * homograph[2][1]);
    tempMat[0][1] = -((homograph[1][0] * homograph[2][2]) -
    (homograph[1][2] * homograph[2][0]));
    tempMat[0][2] =
    (homograph[1][0] * homograph[2][1]) - (homograph[1][1] * homograph[2][0]);
    tempMat[1][0] = -((homograph[0][1] * homograph[2][2]) -
    (homograph[0][2] * homograph[2][1]));
    tempMat[1][1] =
    (homograph[0][0] * homograph[2][2]) - (homograph[0][2] * homograph[2][0]);
    tempMat[1][2] = -((homograph[0][0] * homograph[2][1]) -
    (homograph[0][1] * homograph[2][0]));
    tempMat[2][0] =
    (homograph[0][1] * homograph[1][2]) - (homograph[0][2] * homograph[1][1]);
    tempMat[2][1] = -((homograph[0][0] * homograph[1][2]) -
    (homograph[0][2] * homograph[1][0]));
    tempMat[2][2] =
    (homograph[0][0] * homograph[1][1]) - (homograph[0][1] * homograph[1][0]);
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < i; j++) {
            double tmp = tempMat[i][j];
            tempMat[i][j] = tempMat[j][i];
            tempMat[j][i] = tmp;
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            homograph[i][j] = (tempMat[i][j] / dtm);
        }
    }
}


/*Part 2.1 (works both for homograph between two images as wells as among images*/
int part2_func(vector<DistProcesdDesciptr*> siftMatchVectorOneToOne) {
    double homograph[8][1],sum = 0.0, dist_threshold = 200, p_succ = 0.95,
    totalDistance = 0;
    int inliers = 0, bestInliers = 0, distCount = 0;
    vector<DistProcesdDesciptr*> inBetWeenInliers;
    double randArr[4];
    /*select random 4 pairs of descriptors*/
    double rand_vect1[4][2], rand_vect2[4][2];
    int noOfTimes = 200, maxNumOfTimes = -1;
    for (int num = 0; num < 400; num++) {
        inBetWeenInliers.clear();
        inliers = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2;) {
                int rand_val = rand() % siftMatchVectorOneToOne.size();
                rand_vect1[i][j++] = siftMatchVectorOneToOne.at(rand_val)->getCoordImage1_col();
                rand_vect1[i][j--] = siftMatchVectorOneToOne.at(rand_val)->getCoordImage1_row();
                rand_vect2[i][j++] = siftMatchVectorOneToOne.at(rand_val)->getCoordImage2_col();
                rand_vect2[i][j++] = siftMatchVectorOneToOne.at(rand_val)->getCoordImage2_row();
            }
        }
        double matrix[8][8] = {
            {
                rand_vect1[0][0], rand_vect1[0][1], 1, 0, 0, 0,
                -(rand_vect1[0][0] * rand_vect2[0][0]),
                -(rand_vect1[0][1] * rand_vect2[0][0])
            }
            ,
            {
                0, 0, 0, rand_vect1[0][0], rand_vect1[0][1], 1,
                -(rand_vect1[0][0] * rand_vect2[0][1]),
                -(rand_vect1[0][1] * rand_vect2[0][1])
            }
            ,
            {
                rand_vect2[1][0], rand_vect1[1][1], 1, 0, 0, 0,
                -(rand_vect1[1][0] * rand_vect2[1][0]),
                -(rand_vect1[1][1] * rand_vect2[1][0])
            }
            ,
            {
                0, 0, 0, rand_vect1[1][0], rand_vect1[1][1], 1,
                -(rand_vect1[1][0] * rand_vect2[1][1]),
                -(rand_vect1[1][1] * rand_vect2[1][1])
            }
            ,
            {
                rand_vect1[2][0], rand_vect1[2][1], 1, 0, 0, 0,
                -(rand_vect1[2][0] * rand_vect2[2][0]),
                -(rand_vect1[2][1] * rand_vect2[2][0])
            }
            ,
            {
                0, 0, 0, rand_vect1[2][0], rand_vect1[2][1], 1,
                -(rand_vect1[2][0] * rand_vect2[2][1]),
                -(rand_vect1[2][1] * rand_vect2[2][1])
            }
            ,
            {
                rand_vect1[3][0], rand_vect1[3][1], 1, 0, 0, 0,
                -(rand_vect1[3][0] * rand_vect2[3][0]),
                -(rand_vect1[3][1] * rand_vect2[3][0])
            }
            ,
            {
                0, 0, 0, rand_vect1[3][0], rand_vect1[3][1], 1,
                -(rand_vect1[3][0] * rand_vect2[3][1]),
                -(rand_vect1[3][1] * rand_vect2[3][1])
            }
        };
        double** retMat = new double*[8];
        for (int i = 0; i < 8; i++) retMat[i] = new double[8];
        // cout << "Matrix value before inverting: "<<retMat[5][5]<<"
        // "<<retMat[1][4]<<endl;
        cofactor(matrix, 8, retMat);
        // cout << "Matrix value after inverting: "<<retMat[5][5]<<"
        // "<<retMat[1][4]<<endl;
        double coords[8][1] = {
            {
                rand_vect2[0][0]
            }
            , {
                rand_vect2[0][1]
            }
            ,
            {
                rand_vect2[1][0]
            }
            , {
                rand_vect2[1][1]
            }
            ,
            {
                rand_vect2[2][0]
            }
            , {
                rand_vect2[2][1]
            }
            ,
            {
                rand_vect2[3][0]
            }
            , {
                rand_vect2[3][1]
            }
        };
        /*Matrix multiplication*/
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < 8; k++) {
                    sum += sum + retMat[i][k] * coords[k][j];
                }
                homograph[i][j] = sum;
                sum = 0.0;
            }
        }
        double transHomogGraph[3][3];
	transHomogGraph[0][0] = homograph[0][0];
        transHomogGraph[0][1] = homograph[1][0];
        transHomogGraph[0][2] = homograph[2][0];
        transHomogGraph[1][0] = homograph[3][0];
        transHomogGraph[1][1] = homograph[4][0];
        transHomogGraph[1][2] = homograph[5][0];
        transHomogGraph[2][0] = homograph[6][0];
        transHomogGraph[2][1] = homograph[7][0];
        transHomogGraph[2][2] = 1;
        //cout << "Test: Matrix value before inverting:"<<transHomogGraph[1][2]<<" "<<transHomogGraph[2][1]<<endl;
        inverse(transHomogGraph);
        //cout << "Test: Matrix value after inverting:"<<transHomogGraph[1][2]<<" "<<transHomogGraph[2][1]<<endl;
        for (int i = 0; i < siftMatchVectorOneToOne.size(); i++) {
            bool alreadyPerformed = false;
            for (int j = 0; j < 4; j++) {
                if (i == randArr[j]) {
                    alreadyPerformed = true;
                }
            }
            if (!alreadyPerformed) {
                DistProcesdDesciptr* ivc = siftMatchVectorOneToOne[i];
                double dest_x = ivc->getCoordImage2_col();
                double dest_y = ivc->getCoordImage2_row();
                double src_x =
                ((transHomogGraph[0][0] * dest_x) +
                (transHomogGraph[0][1] * dest_y) + transHomogGraph[0][2]) /
                ((transHomogGraph[2][0] * dest_x) +
                (transHomogGraph[2][1] * dest_y) + 1);
                double src_y =
                ((transHomogGraph[1][0] * dest_x) +
                (transHomogGraph[1][1] * dest_y) + transHomogGraph[1][2]) /
                ((transHomogGraph[2][0] * dest_x) +
                (transHomogGraph[2][1] * dest_y) + 1);
                double distance = sqrt(pow((ivc->getCoordImage1_col() - src_x), 2) +
                pow((ivc->getCoordImage1_row() - src_y), 2));
                //cout << "Distance : " << distance<<endl;
                totalDistance += distance;
                distCount++;
                if (distance <= dist_threshold) {
                    inliers++;
                    DistProcesdDesciptr* f1;
                    f1 = new DistProcesdDesciptr;
                    f1->setImageName1(ivc->setImageName_1);
                    f1->setImageName2(ivc->setImageName_2);
                    f1->setCoordImage1(ivc->getCoordImage1_col(), ivc->getCoordImage1_row());
                    f1->setCoordImage2(dest_x, dest_y);
                    inBetWeenInliers.push_back(f1);
                }
            }
            /*if(maxNumOfTimes<=-1){
                maxNumOfTimes =
                (1-p_succ)/(log2(1-pow((1-((siftMatchVectorOneToOne.size()-inliers)/siftMatchVectorOneToOne.size())),4)));
                cout << "Max num of times : "<<maxNumOfTimes;
            }
            */
        }
        if (inliers >= 1 && inliers > bestInliers) {
            bestInliers = inliers;
            bestVectInliers.clear();
            bestVectInliers = inBetWeenInliers;
            cout << "best vector size : " << bestVectInliers.size() << endl;
            cout << "best till now :" << bestInliers << endl;
            best_homograph[0][0] = homograph[0][0];
            best_homograph[0][1] = homograph[1][0];
            best_homograph[0][2] = homograph[2][0];
            best_homograph[1][0] = homograph[3][0];
            best_homograph[1][1] = homograph[4][0];
            best_homograph[1][2] = homograph[5][0];
            best_homograph[2][0] = homograph[6][0];
            best_homograph[2][1] = homograph[7][0];
            best_homograph[2][2] = 1;
        }
    }
    cout << "\n part 2 finished"<<endl;
    return bestVectInliers.size();
}


void Assignment2Part2PrecisionCalulcation(vector<string> argv, vector<string> matchImages){
                                                    
int counter=0;

	for(int i=0; i< argv.size()-1; i=i+1){
		 MatchedSwiftVectorsOfSingleImage.clear();
		if(argv[0].compare(argv[i+1])==0){
			cout<<"matched  ";cout<< argv[0];cout<<argv[i+1];continue;}
		else{
		 	compareImages(argv[0],argv[i+1],i);
		 	if(MatchedSwiftVectorsOfSingleImage.size()>0){
				int size = part2_func(MatchedSwiftVectorsOfSingleImage);
				swiftMatchedVectCount *s1;
				s1 = new swiftMatchedVectCount;
				s1->setImageName1(argv[0]);
				s1->setImageName2(argv[i+1]);
				s1->setTotalMatchesBetweenImage1and2(size);

				FinalResultOfBestMatchImages.push_back(s1);
				FinalStoreOfAllMatchesAllImages.push_back(MatchedSwiftVectorsOfSingleImage);
				//createTheDescriptorJoinViz(argv[0],argv[i+1],MatchedSwiftVectorsOfSingleImage);
			     }
			counter = counter+1;
			cout << "Finished Executing ::  "<<counter << "  ::  files over the given query image"<< endl;
			cout << "The last processed image is :  "<< argv[i+1] << endl;
		}
	}

			// sort the resultant image results.
			// Now we should sort by total matches
			//and
			// lets caclulate the total precision value
        std::sort( FinalResultOfBestMatchImages.begin(),  FinalResultOfBestMatchImages.end(), swiftMatchedVectCount::compareMyDataPredicate);
        int counterOfBestMatches = 0;
        int precesionMatches=0;       
	for (vector<swiftMatchedVectCount *> ::iterator ivc = FinalResultOfBestMatchImages.begin(); ivc != FinalResultOfBestMatchImages.end(); ++ivc) {
             counterOfBestMatches = counterOfBestMatches+1;
             cout << "Match Number :: " << counterOfBestMatches << endl;
	     if (counterOfBestMatches > 10){break;}
             (*ivc)->printswiftMatchedVectCount();
	     //(*ivc)->getImageName2();		
			//cout << "inside checking the while";
			for(int kl=0; kl< toCheckTop10NamesOfFiles.size(); kl++){
				//string name = (string)(*ivc)->getImageName2();
				if(((string)(*ivc)->getImageName2()).compare(toCheckTop10NamesOfFiles[kl]) == 0){
					precesionMatches++;
					//toCheckTop10NamesOfFiles.erase((string)toCheckTop10NamesOfFiles[kl]);
					//cout << "To remove" ;
					cout << "In top 10- of same category!! "<<toCheckTop10NamesOfFiles[kl];
					//cout  << " and removed ";
					//cout  <<  toCheckTop10NamesOfFiles.begin()+kl << endl;
					//toCheckTop10NamesOfFiles.erase(toCheckTop10NamesOfFiles.begin()+kl);
					//break;
				}
			} 
		
	}
	cout << "total argc count   " << argv.size() << endl;
	 std::cout << "The total precision for Query :" << argv[0] << "  is  ::: " << (precesionMatches*100)/toCheckTop10NamesOfFiles.size()<< endl;
}

void precision(int argc, char **argv, string part){


//cout << argv[2];
		vector<string> argumentsForPrecisionCalculationFileNames;
		argumentsForPrecisionCalculationFileNames.push_back((string)argv[2]);
		//argumentsForPrecisionCalculationFileNames.push_back("part1-part2");
		

		// For precision claculation, the input should be changed
  		DIR*     dir;
  		dirent*  pdir;
  		dir = opendir("./a2-images/part1_images/");     // open current directory
		cout << "opened the directory "<< endl;
		int escape_two_files = 1;
  		while (pdir = readdir(dir)) {
			if(escape_two_files < 3){
                        	escape_two_files = escape_two_files+1;
				continue;		
			}
        		//cout << pdir->d_name << endl;
			string nametoCheck = "a2-images/part1_images/" +(string) pdir->d_name;
			//cout << "currently checking   " + nametoCheck;
			//cout << "argv 2  " <<(string)argv[2];
			
			if (nametoCheck.compare((string)argv[2])!=0){
			
				std::string extracted_nametoCheck;
				std::string extracted_QuerynametoCheck;
				//cout << "strings didnt match!!"<<endl;
				extracted_nametoCheck = nametoCheck.substr(0, nametoCheck.find_last_of("_"));
				//cout << "extracted_nametoCheck " << extracted_nametoCheck << endl;
				extracted_QuerynametoCheck = ((string)argv[2]).substr(0, ((string)argv[2]).find_last_of("_"));
				//cout << " extracted_QuerynametoCheck " << extracted_QuerynametoCheck << endl;
				if (extracted_nametoCheck.compare(extracted_QuerynametoCheck) == 0) {
					toCheckTop10NamesOfFiles.push_back(nametoCheck);
					//cout << "toCheckTop10NamesOfFiles : "<<endl;
					//cout << nametoCheck << endl;
					//cout << toCheckTop10NamesOfFiles.size() <<endl;
				}

				argumentsForPrecisionCalculationFileNames.push_back(nametoCheck);
				//cout << "argumentForPrecisionCalculation : ";
				//cout << argumentsForPrecisionCalculationFileNames.size() << endl;
			  }
			else{cout << "matched  "; cout << nametoCheck <<endl; }
 	      		
		}
		closedir(dir);
		cout << "Total files to process : "<<argumentsForPrecisionCalculationFileNames.size()-1 << endl;
		cout << " number of files belong to query category "<< toCheckTop10NamesOfFiles.size() <<endl;
		if(part == "part1"){
			Assignment2Part1PrecisionCalulcation(argumentsForPrecisionCalculationFileNames, toCheckTop10NamesOfFiles);
		}else if(part == "part2"){
			Assignment2Part2PrecisionCalulcation(argumentsForPrecisionCalculationFileNames, toCheckTop10NamesOfFiles);
		}

}



string returnFileNameGivenPath(string path){
    if(path.find("/") != string::npos)
    path = (path).substr(path.find_last_of("/")+1,path.size()-1);
    path = path.substr(0,path.find_last_of("."))+"-warped."+path.substr(path.find_last_of(".")+1,path.size()-1);
    return path;
}
/*Part 3 : Creates warped image given homography matrix and image to be warped*/

int get_cubic_val(int x){

}
/*void bicubic_interpolation(CImg img, int x, int y){
	int q=0;
	for(int j=0;j<3;j++){
		int v = y+j-1, p=0;
		for(int i=0;i<3;i++){
			int u=x+i-1;
			p+=output_image(x,y)*
		}
	}
}*/

void part3_func(double someHomograph[3][3],string path) {
    for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
    cout <<"Estimated homography matrix : "<< someHomograph[i][j]<<" "<<endl;
    CImg<double> input_image(path.c_str());
    CImg<double> output_image(input_image.width(), input_image.height(), 1, 3,
    0);
    inverse(someHomograph);
    for (int col = 0; col < input_image.width(); col++) {
        for (int row = 0; row < input_image.height(); row++) {
            double src_x =
            ((someHomograph[0][0] * col) + (someHomograph[0][1] * row) + someHomograph[0][2]) /
            ((someHomograph[2][0] * col) + (someHomograph[2][1] * row) + someHomograph[2][2]);
            double src_y =
            ((someHomograph[1][0] * col) + (someHomograph[1][1] * row) + someHomograph[1][2]) /
            ((someHomograph[2][0] * col) + (someHomograph[2][1] * row) + someHomograph[2][2]);
            if (src_x >= 0 && src_y >= 0 && src_x < input_image.width() &&
            src_y < input_image.height()) {
                output_image(col, row, 0, 0) = input_image(src_x, src_y, 0, 0);
                output_image(col, row, 0, 1) = input_image(src_x, src_y, 0, 1);
                output_image(col, row, 0, 2) = input_image(src_x, src_y, 0, 2);
            }
            else {
                output_image(col, row, 0, 0) = 0;
                output_image(col, row, 0, 1) = 0;
                output_image(col, row, 0, 2) = 0;
            }
        }
    }
    string fiName = returnFileNameGivenPath(path);
    output_image.save(fiName.c_str());
    cout << "Image file saved as : "+fiName<<endl;
}
void part3_sequenceWarp(string queryImg, string testImg){
    MatchedSwiftVectorsOfSingleImage.clear();
    compareImages(queryImg,testImg,0);
}



vector<ReducedVector*> reduceK(vector<SiftDescriptor> image1Descriptor)
{
	//vector<vector<ReducedVector*> > test;
	vector<ReducedVector*> descriptorVector;
	//vector<SiftDescriptor> image1Descriptor = AllImageStoredDescriptors[0];
	vector<double> x;
	for(int i=0;i<image1Descriptor.size();i++)
	{
		SiftDescriptor tempDescriptor = image1Descriptor[i];
		vector<double> tempVector;
		for(int count = 0;count<n;count++)
		{
			double tempDescSum = 0;
			x = RandomVector[count];
			for(int j=0;j<128;j++)
			{
				tempDescSum+=x[j]*tempDescriptor.descriptor[j];
			}
			double f_val = (double) tempDescSum/w;
			tempVector.push_back(f_val);		
		}
		
		ReducedVector *f1;
		f1 = new ReducedVector;
		f1->setX(tempDescriptor.col);
		f1->setY(tempDescriptor.row);
		f1->setAllDescriptors(tempVector);
		f1->setIndex(i);
		//f1->printReducedVector();
		descriptorVector.push_back(f1);
		tempVector.clear();
	}	
	
	return descriptorVector;
	
}

double CalculateEucledianBetweenVectors(ReducedVector* descVec1, ReducedVector* descVec2) {
  double eucledianDistanceValue = 0;
  vector<double> desc1 = descVec1->getAllDescriptors();
  vector<double> desc2 = descVec2->getAllDescriptors();
  //cout<<"calculating the eucledian distance of 2 vectors" <<endl;
  for (int l = 0; l <n; l++) {
    eucledianDistanceValue = eucledianDistanceValue + pow(desc1[l] - desc2[l], 2);
  }

  return sqrt(eucledianDistanceValue);
}

SiftDescriptor getSiftFromReduced(ReducedVector* rVector, vector<SiftDescriptor> originalDesc)
{
	/*SiftDescriptor tempSiftDesc;
	for(int i=0;i<originalDesc.size();i++)
	{
		if(originalDesc[i].row == rVector->getY() && originalDesc[i].col == rVector->getX())
			tempSiftDesc = originalDesc[i];
	}
	return tempSiftDesc;*/
	return originalDesc[rVector->getIndex()];
}

void compareOriginalDescriptors(ReducedVector* img1Vector, vector<ReducedVector*> img2Vectors, vector<SiftDescriptor> img1Desc, vector<SiftDescriptor> img2Desc)
{
	SiftDescriptor img1Sift = getSiftFromReduced(img1Vector, img1Desc);
	vector<SiftDescriptor> img1SiftVectors, img2SiftVectors;
	img1SiftVectors.push_back(img1Sift);
	for(int i = 0;i<2;i++)
	{
		SiftDescriptor img2Sift = getSiftFromReduced(img2Vectors[i],img2Desc);
		img2SiftVectors.push_back(img2Sift);
	}
	
	MatchGivenTwoImageDescriptors(img1SiftVectors,img2SiftVectors,strImg1,strImg2);

}

void compareImagesForReducedVector(vector<ReducedVector*> img1Vector, vector<ReducedVector*> img2Vector, vector<SiftDescriptor> img1OriginalDesc, vector<SiftDescriptor> img2OriginalDesc)
{
	MatchedSwiftVectorsOfSingleImage.clear();
	for(int i=0;i<img1Vector.size();i++)
	{
		ReducedVector* img1Desc = img1Vector[i];
		vector<ReducedVector*> allVectors;
		for(int j=0;j<img2Vector.size();j++)
		{
			ReducedVector* img2Desc = img2Vector[j];
			double tempDistance = CalculateEucledianBetweenVectors(img1Desc,img2Desc);
			img2Desc->setEucledianDistanceValue(tempDistance);
			allVectors.push_back(img2Desc);
		}
		std::sort(allVectors.begin(), allVectors.end(), ReducedVector::compareMyDataPredicate);
		compareOriginalDescriptors(img1Desc,allVectors,img1OriginalDesc,img2OriginalDesc);
		allVectors.clear();
	}
	// counting the total matches and inserting into objects
	if (MatchedSwiftVectorsOfSingleImage.size()> 0) {
	  swiftMatchedVectCount * s1;
	  s1 = new swiftMatchedVectCount;
	  s1 -> setImageName1(strImg1);
	  s1 -> setImageName2(strImg2);
	  s1 -> setTotalMatchesBetweenImage1and2(MatchedSwiftVectorsOfSingleImage.size());

	  FinalResultOfBestMatchImages.push_back(s1);
	  FinalStoreOfAllMatchesAllImages.push_back(MatchedSwiftVectorsOfSingleImage);
	  createTheDescriptorJoinViz(strImg1, strImg2, MatchedSwiftVectorsOfSingleImage,"summarizeVectorResult.jpg");
	}
}

void ReduceImageDescriptors(string image1, string image2)
{
	strImg1 = image1;
	strImg2 = image2;
	vector<double> x;
	vector<ReducedVector*> imgReducedVector;
	vector<ReducedVector*> imgReducedVector2;
	for(int count = 0;count<n;count++)
	{
		for(int i=0;i<128;i++)
			x.push_back((double) rand() / (RAND_MAX));
		RandomVector.push_back(x);
		x.clear();
	}
	
	vector<SiftDescriptor> image1Descriptor = createSiftDescriptor(image1);
	imgReducedVector = reduceK(image1Descriptor);
	vector<SiftDescriptor> image2Descriptor = createSiftDescriptor(image2);
	imgReducedVector2 = reduceK(image2Descriptor);
	compareImagesForReducedVector(imgReducedVector,imgReducedVector2, image1Descriptor,image2Descriptor);
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
    //string inputFile = argv[2];
   
	if(part == "part1"){
		// This is just a bit of sample code to get you started, to
		// show how to use the SIFT library.
  
	// ##############################################
		if(argc>3){Assignment2PartABCalulcation(argc, argv);}
	//###############################################
		if(argc==3){	precision(argc, argv, part);}
	//###############################################	

       }
    

    else if(part == "part2")
        {
            if(argc > 3){
                cout << "Calling part 2.1.1 Estimation of homography between just 2 images"<<endl;
                part3_sequenceWarp(argv[2], argv[3]);
                cout << "MatchedSwiftVectorsOfSingleImage size: "<<MatchedSwiftVectorsOfSingleImage.size()<<endl;
                if(MatchedSwiftVectorsOfSingleImage.size() >= 1){
                    part2_func(MatchedSwiftVectorsOfSingleImage);
		    createTheDescriptorJoinViz(bestVectInliers[0]->setImageName_1,bestVectInliers[0]->setImageName_2,bestVectInliers,"ransac_visualized.jpg");
		    cout<<"Image saved as ransac_visualized.jpg"<<endl;
                }
                else{
                    cout << "No sift vectors matched "<<endl;
                }
            }else if(argc == 3){
		cout << "Precision checking using RANSAC"<<endl;
		precision(argc, argv, part);	
	    }
        }
		else if (part == "part2.2") {
			
		ReduceImageDescriptors(argv[2],argv[3]);
		
    }
        else if(part == "part3")
        {
            if(argc == 3){
		cout << "Image warping with given homography matrix : "<<endl;
                double homogen[3][3] = {
                    {
                        0.907, 0.258, -182
                    }
                    ,
                    {
                        -0.153, 1.44, 58
                    }
                    ,
                    {
                        -0.000306, 0.000731, 1
                    }
                };
                part3_func(homogen, argv[2]);
            }
            else if(argc>3){
		cout << "Image sequence warping : "<<endl;
                for(int i=2;i<argc-1;i++){	
                    part3_sequenceWarp(argv[2], argv[i+1]);
                    cout << "MatchedSwiftVectorsOfSingleImage size: "<<MatchedSwiftVectorsOfSingleImage.size()<<endl;
                    if(MatchedSwiftVectorsOfSingleImage.size() >= 1){
                        int size = part2_func(MatchedSwiftVectorsOfSingleImage);
                        part3_func(best_homograph, argv[i+1]);
                    }
                    else{
                        cout << "No sift vectors matched "<<endl;
                    }
                }
            }
        } else if(part == "test"){
		string inFile = argv[2];
		CImg<double> in_image(inFile.c_str());
		vector<SiftDescriptor> descriptors = createSiftDescriptor(inFile.c_str());
		for(int i=0; i<descriptors.size(); i++){
			for(int j=0; j<5; j++)
		      		for(int k=0; k<5; k++)
					if(j==2 || k==2)
		  				for(int p=0; p<3; p++)
		    					in_image(descriptors[i].col+k, descriptors[i].row+j, 0, p)=0;
	  	}
		in_image.get_normalize(0, 255).save("sift.png");
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
