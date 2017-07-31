#include <iostream>
#include <vector>
#include <string>

using namespace std;

class swiftMatchedVectCount
{
    public:
        //static bool compareMyDataPredicate(swiftMatchedVectCount lhs, swiftMatchedVectCount rhs) { return (lhs.EucledianDistanceValue < rhs.EucledianDistanceValue); }
        swiftMatchedVectCount();

        static bool compareMyDataPredicate( swiftMatchedVectCount* lhs,  swiftMatchedVectCount* rhs)
        {
                return lhs->getTotalMatchesBetweenImage1and2() > rhs->getTotalMatchesBetweenImage1and2();
        }

        void setImageName1(string imageName_1){setImageName_1 = imageName_1;}
        void setImageName2(string imageName_2){setImageName_2 = imageName_2;}

        string getImageName1(){return setImageName_1; }
        string getImageName2(){return setImageName_2;}

        void  printswiftMatchedVectCount(){
                cout << "<setImageName1  , setImageName2      :< " << setImageName_1 <<"," << setImageName_2<<">"<< endl;
                cout << "TotalMatchesBetweenImage1and2     : " << TotalMatchesBetweenImage1and2 << endl <<endl;
                }


        int getTotalMatchesBetweenImage1and2(){return TotalMatchesBetweenImage1and2;}
        void setTotalMatchesBetweenImage1and2(int totalmatches){TotalMatchesBetweenImage1and2=totalmatches;}

    private:
        string setImageName_1;
        string setImageName_2;
        int  TotalMatchesBetweenImage1and2;
};


//implementations
swiftMatchedVectCount::swiftMatchedVectCount(){
        TotalMatchesBetweenImage1and2 = 0;
 }
    
