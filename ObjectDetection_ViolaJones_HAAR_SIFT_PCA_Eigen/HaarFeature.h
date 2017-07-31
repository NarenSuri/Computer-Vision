#include <map>
#include <ImgMask.h>
template <class T>
class HaarFeature{
	public:
	static const int subsample_size = 30;
	string _algo;
	string _mode;
	int counter;
	multimap<int, vector<string> > fileWriteMap;
	vector<ImgMask*> maskVector;
	void setAlgo(string algo){
		_algo = algo;
	}
	void setMode(string mode){
		_mode = mode;
	}
	/*http://stackoverflow.com/questions/1123201/convert-double-to-string-c*/
	string double_to_string(double val){
		stringstream s;
		s<<val;
		return s.str();
	}

	string int_to_string(int val){
		stringstream s;
		s<<val;
		return s.str();
	}
	
	CImg<double> subsample(const string &filename) {
		return (CImg<double>(filename.c_str())).resize(subsample_size,
				subsample_size, 1, 3);
	}
	
	void createCascade(int row, int col, vector< vector<int> > values){
		ImgMask *c = new ImgMask(row,col);
		for(int i=0;i<row;i++){
			for(int j=0;j<col;j++){
				c->imgMask[i][j] = values[i][j];
			}
		}
		maskVector.push_back(c);
	}
	
	int generateRand(int min, int max){
		return (min + ( std::rand() % ( max - min + 1 ) ));
	}
	
	void generateRandomCascade(){
		int maskCount = 300;		//no. of masks to be generated
		int cValues[2] = {-1,1};		//-1 for black and 1 for white
		for(int i=0;i<maskCount;i++){
			int row = generateRand(1,20);
			int colCount = 2;
			if(row>1)	
				colCount = 1;	//if there are more than one rows, column can be one
			int col = generateRand(colCount,20);
			vector< vector<int> > values;
			for(int j=0;j<row;j++){
				vector<int> rowValues;
				for(int k=0;k<col;k++)
				{
					rowValues.push_back(cValues[generateRand(0,1)]);
				}
				values.push_back(rowValues);
			}
			createCascade(row,col,values);
		}
	}
	
	vector<string> performHaar(CImg <double> &img){
		//cout<<"Imgc: "<<imgc++<<endl;
		counter = 1;
		vector<string> featurVec;
		for(int vcount = 0; vcount<maskVector.size();vcount++)
		{
			ImgMask* cas = maskVector[vcount];
			for(int i=0;i<img.width()-cas->col;i++)
			{
				for(int j=0;j<img.height()-cas->row;j++)
				{
					double val1=0, val2=0;
					for(int k=0;k<cas->row;k++)
					{
						for(int l=0;l<cas->col;l++)
						{
							if(cas->imgMask[k][l] == -1)
								val1+=img.atXY(i+k,j+l);
							else
								val2+=img.atXY(i+k,j+l);
						}
					}
					featurVec.push_back(" "+int_to_string(counter++)+":"+double_to_string(val1-val2));
				}
			}
		}
		return featurVec;
		
	}
	void subsample_convert(const Dataset &filenames){
		int class_num = 1;
		//vector <double> d_dim_vector(subsample_size * subsample_size);
		for (Dataset::const_iterator c = filenames.begin();
						c != filenames.end(); ++c) {
			for (int i = 0; i < c->second.size(); i++) {
				CImg<double> subsampled = subsample(c->second[i]);
				//d_dim_vector = imgToVector(subsampled);
				vector<string> resultVector = performHaar(subsampled);
				if(resultVector.size()>0)
					fileWriteMap.insert(make_pair(class_num, resultVector));
			}
			++class_num;
		}
	}
	void execute_command(string command){
		const char* command_str = command.c_str();
		system(command_str);
	}
	
	void write_map_to_file(string filename) {
		cout<<"Haar Feature | write_map_to_file :: info : inside function"<<endl;

			ofstream feature_file(filename.c_str());
			if (feature_file.is_open()) {
				//remove(filename.c_str());
				feature_file << "# Feature file for " << _algo << endl;
				multimap<int, vector<string> >::iterator map_iter;
				for (map_iter = fileWriteMap.begin();
						map_iter != fileWriteMap.end(); ++map_iter) {
					feature_file << map_iter->first;
					for (int i = 0; i < map_iter->second.size(); i++) {
						feature_file << map_iter->second[i];
					}
					feature_file << endl;
				}
				feature_file.close();
			}
	}
	
	void computeHaar(const Dataset &filenames){
		generateRandomCascade();
		counter = 1;
		string outputFile = _algo + _mode + ".dat";
			fileWriteMap.clear();
			cout<<"Starting to compute Haar Like. Mode: "<<_mode<<endl;
			subsample_convert(filenames);
			write_map_to_file(outputFile);
			cout<<"Performing SVM"<<endl;
			if(_mode == "test")
				execute_command("./svm_multiclass_classify "+outputFile+" "+_algo+"_model.dat"+" "+_algo+"_output.dat");
			else
				execute_command("./svm_multiclass_learn -c 1.0 "+outputFile+" "+_algo+"_model.dat"+" "+_algo+"_output.dat");
	}

};
