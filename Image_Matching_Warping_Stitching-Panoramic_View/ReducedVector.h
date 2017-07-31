#include <iostream>
#include <vector>
#include <string>

using namespace std;

class ReducedVector
{
    public:
        ReducedVector();

		static bool compareMyDataPredicate( ReducedVector* lhs,  ReducedVector* rhs)
		{
    		return lhs->getEucledianDistanceValue() < rhs->getEucledianDistanceValue();
		}
        void setX(double val){x = val;}
        void setY(double val){y = val;}
		void setIndex(int index){siftIndex = index;}
		
		
        double getX(){return x; }
        double getY(){return y;}
		int getIndex(){return siftIndex;}
		double  getEucledianDistanceValue(){
			return EucledianDistanceValue;}
        void  printReducedVector(){
                cout << "<X  , Y      :< " << x <<"," << y<<">"<< endl;
				cout << "size of my vec" << AllVecAtDescriptors.size()<< endl;
				for(int i=0;i<AllVecAtDescriptors.size();i++)
				{
					cout<<AllVecAtDescriptors[i]<< "\t";
				}
                      }


        vector<double>  getAllDescriptors(){return AllVecAtDescriptors;}
        void setAllDescriptors(vector <double> AllVecAtDescriptor){AllVecAtDescriptors=AllVecAtDescriptor;}
		void setEucledianDistanceValue(double EucledianDistance){EucledianDistanceValue = EucledianDistance;}

    private:
        double x;
		double y;
		int siftIndex;
        vector <double> AllVecAtDescriptors;
		double EucledianDistanceValue;
};

//implementations
ReducedVector::ReducedVector(){
         }



    
