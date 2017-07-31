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
#include <fstream>
#include <sstream>
//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

int main(int argc, char** argv)
{
    try {

        if (argc < 2) {
            cout << "Insufficent number of arguments; correct usage:" << endl;
            cout << "    a2 part_id ..." << endl;
            return -1;
        }

        string part = argv[1];
        string inputFile = argv[2];

        if (part == "part1") {
            // This is just a bit of sample code to get you started, to
            // show how to use the SIFT library.

            //CImg<double> input_image(inputFile.c_str());

            ifstream infile;
            infile.open("b657-wars.txt");
            string str;
            int i = 0, z = 0.1, d = 10, j = 0, k = 0, count=0;
            double trans_rot[3][4] = { { 1, 0, 0, 0 },
                { 0, 1, 0, 0 },
                { 0, 0, 1, 0.1 } };
            while (getline(infile, str)) {
                istringstream instr(str);
                string s;
               double result[3][1] = {{0},{0},{0}}, arr[4][1];
                while (getline(instr, s, ' ')) {
                    arr[i++][0] = atof(s.c_str());
                }
                arr[3][0] = 1;
                for (i = 0; i < 3; ++i){
                    for (j = 0; j < 1; ++j){
                        for (k = 0; k < 4; ++k) {
			   // cout<<"i : "<<i<<", k: "<<k<<endl;
			    cout << "trans : "<<trans_rot[i][k];
			    cout<<", arr : "<<arr[k][j]<<endl;
                            result[i][j] += trans_rot[i][k] * arr[k][j];
                        }
			cout << "inter : "<<result[i][j]<<endl;
		   }	
		}

		++count;
		if(count > 5){
			exit(0);
		}
	   }
    }
    else if (part == "part2")
    {
        // do something here!
    }
    else if (part == "part3")
    {
        // do something here!
    }
    else throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
}
catch (const string& err)
{
    cerr << "Error: " << err << endl;
}
}
