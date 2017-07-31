#include <iostream>
#include <vector>
#include <string>

using namespace std;

class DistProcesdDesciptr                    
{
    public:
	//static bool compareMyDataPredicate(DistProcesdDesciptr lhs, DistProcesdDesciptr rhs) { return (lhs.EucledianDistanceValue < rhs.EucledianDistanceValue); }
	DistProcesdDesciptr();
        
	static bool compareMyDataPredicate( DistProcesdDesciptr* lhs,  DistProcesdDesciptr* rhs)
	{
    		return lhs->getEucledianDistanceValue() < rhs->getEucledianDistanceValue();
	}

	void setImageName1(string imageName_1){setImageName_1 = imageName_1;}
        void setImageName2(string imageName_2){setImageName_2 = imageName_2;}
        
	void setCoordImage1(float DescriptrCord_1, float DescriptrCord_2){
        	Image_1_DescriptrCord_1 = DescriptrCord_1;
        	Image_1_DescriptrCord_2 = DescriptrCord_2;}

        void setCoordImage2(float DescriptrCord_1, float DescriptrCord_2){
        	Image_2_DescriptrCord_1 = DescriptrCord_1;
        	Image_2_DescriptrCord_2 = DescriptrCord_2;}
	
	void setEucledianDistanceValue(float EucledianDistance){EucledianDistanceValue = EucledianDistance;}
	

	float getCoordImage1_col(){return Image_1_DescriptrCord_2; }
	float getCoordImage1_row(){return Image_1_DescriptrCord_1; }

	float getCoordImage2_col(){return Image_2_DescriptrCord_2; }
	float getCoordImage2_row(){return Image_2_DescriptrCord_1; }



	float  getEucledianDistanceValue(){
		return EucledianDistanceValue;}

	
	void  printDistProcesdDesciptr(){
		cout << "<setImageName1  , setImageName2      :< " << setImageName_1 <<"," << setImageName_2<<">"<< endl;
 		cout << "<Image_1_DescriptrCord_1  , Image_1_DescriptrCord_2      :< " << Image_1_DescriptrCord_1 <<"," << Image_1_DescriptrCord_2<<">"<< endl;
		cout << "<Image_2_DescriptrCord_1  , Image_2_DescriptrCord_2      :< " << Image_2_DescriptrCord_1 <<"," << Image_2_DescriptrCord_2<<">"<< endl;
    		cout << "EucledianDistanceValue     : " << EucledianDistanceValue << endl <<endl;
		}
	void setIsSwiftVectorsMatched(){IsSwiftVectorsMatched = true;}
	bool getIsSwiftVectorsMatched(){return IsSwiftVectorsMatched;}
	


    public:

        string setImageName_1;
        string setImageName_2;
        float Image_2_DescriptrCord_1;
        float Image_2_DescriptrCord_2;
        float Image_1_DescriptrCord_1;
        float Image_1_DescriptrCord_2;
        float EucledianDistanceValue;
	bool IsSwiftVectorsMatched;
	
};


//implementations
DistProcesdDesciptr::DistProcesdDesciptr(){ 
	IsSwiftVectorsMatched = false;
 	}
