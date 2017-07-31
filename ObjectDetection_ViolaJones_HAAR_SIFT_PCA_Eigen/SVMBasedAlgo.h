
#include <map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
template <class T>
class SVMBasedAlgo {
public:
	static const int subsample_size = 80;
	string _algo;
	multimap<int, vector<string> > fileWriteMap;
	SVMBasedAlgo(const string &algo) :  _algo(algo){}

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
	void train(const Dataset &filenames) {
		if (_algo == "baseline") {
			cout<<"SVMBasedAlgo | train :: baseline info : inside function"<<endl;
			baseline_training(filenames);
		} else if(_algo == "deep"){
			cout<<"SVMBasedAlgo | train :: deep info : inside function"<<endl;
			overfeat_training(filenames);
		}
	}

	void test(const Dataset &filenames) {
		if (_algo == "baseline") {
                        cout<<"SVMBasedAlgo | test :: baseline info : inside function"<<endl;
                        baseline_testing(filenames);
                } else if(_algo == "deep"){
			cout<<"SVMBasedAlgo | test :: baseline info : inside function"<<endl;
                        overfeat_testing(filenames);
		}
	}
	
	void call_train_test(string mode, const Dataset &filenames){
		if(mode == "train"){
			train(filenames);
		} else if(mode == "test"){
			test(filenames);
		}
	}

	CImg<double> subsample(const string &filename, const int subsample_size) {
		//cout<<"SVMBasedAlgo | subsample :: info : inside function"<<endl;
		//cout<<"width : "<<CImg<double>(filename.c_str()).width()<<" | height : "<<CImg<double>(filename.c_str()).height()<<endl;
		return (CImg<double>(filename.c_str())).resize(subsample_size,
				subsample_size, 1, 3);
	}

	vector<string> d_dimensional_vector(CImg<double> image, bool is_color) {
		vector < string > d_dim_vector(subsample_size * subsample_size);
		int counter = 1;
		//cout<<"subsampled image : width : "<<image.width()<<" | height : "<<image.height()<<endl;
		for (int col = 0; col < image.width(); col++) {
			for (int row = 0; row < image.height(); row++) {
				if(is_color){
					int R = image(col, row, 0, 0);
					int G = image(col, row, 0, 1);
					int B = image(col, row, 0, 2);
					if(R == 0 && G == 0 && B == 0){
						continue;
					}else{
						d_dim_vector.push_back(int_to_string(counter++) + ":" + double_to_string(R));
						d_dim_vector.push_back(int_to_string(counter++) + ":" + double_to_string(G));
						d_dim_vector.push_back(int_to_string(counter++) + ":" + double_to_string(B));
					}
				}else{
					if (image.atXY(col, row) != 0) {
						d_dim_vector.push_back(int_to_string(counter) + ":" + double_to_string(image.atXY(col, row)));
						++counter;
					}
				}

			}
		}
		return d_dim_vector;
	}

	void write_map_to_file(string filename) {
		cout<<"SVMBasedAlgo | write_map_to_file :: info : inside function"<<endl;

			ofstream feature_file(filename.c_str());
			if (feature_file.is_open()) {
				//remove(filename.c_str());
				feature_file << "# Feature file for " << _algo << endl;
				multimap<int, vector<string> >::iterator map_iter;
				for (map_iter = fileWriteMap.begin();
						map_iter != fileWriteMap.end(); ++map_iter) {
					feature_file << map_iter->first;
					for (int i = 0; i < map_iter->second.size(); i++) {
						feature_file << map_iter->second[i] << " ";
					}
					feature_file << endl;
				}
				feature_file.close();
			}
		}

	void compute(const Dataset &filenames){
		int class_num = 1;
		vector < string > d_dim_vector(subsample_size * subsample_size);
		for (Dataset::const_iterator c = filenames.begin();
						c != filenames.end(); ++c) {
			for (int i = 0; i < c->second.size(); i++) {
				CImg<double> subsampled = subsample(c->second[i], subsample_size);
				d_dim_vector = d_dimensional_vector(subsampled, false);
				fileWriteMap.insert(make_pair(class_num, d_dim_vector));
			}
			++class_num;
		}
	}

	void execute_command(string command){
		const char* command_str = command.c_str();
		system(command_str);
	}

	void baseline_training(const Dataset &filenames) {
		//cout<<"SVMBasedAlgo | baseline_training :: info : inside function"<<endl;
		string filename = _algo + "_training.dat";
		fileWriteMap.clear();
		compute(filenames);
		write_map_to_file(filename);
		execute_command("./svm_multiclass_learn -c 1.0 "+filename+" "+_algo+"_model.dat");
	}

	void baseline_testing(const Dataset &filenames){
		string filename = _algo + "_testing.dat";
		fileWriteMap.clear();
		compute(filenames);
		write_map_to_file(filename);
		execute_command("./svm_multiclass_classify "+filename+" "+_algo+"_model.dat"+" "+_algo+"_output.dat");
	}


/*overfeat code starts*/
	// the overfeat is giving extra spaces in the end 
	// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring  
	// trim from start
	static inline std::string &ltrim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		    std::not1(std::ptr_fun<int, int>(std::isspace))));
	    return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
	    s.erase(std::find_if(s.rbegin(), s.rend(),
		    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	    return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
	    return ltrim(rtrim(s));
	}

	vector<string> get_vector_of_features(int subsample_size){
		vector < string > d_dim_vector(subsample_size * subsample_size);
		ifstream read_from("overfeat.dat");
		string value;
		int counter=1;
		getline(read_from, value);
		while(read_from.good()){
			getline(read_from, value, ' ');
			value = trim(value);
			if(value.size() > 0){
			d_dim_vector.push_back(int_to_string(counter++) + ":" + value); }
		}
		return d_dim_vector;
	}

	void compute_overfeat(const Dataset &filenames){
			int class_num = 1;
			const int sample_size = 231;
			vector < string > d_dim_vector(sample_size * sample_size);
			for (Dataset::const_iterator c = filenames.begin();
							c != filenames.end(); ++c) {
				for (int i = 0; i < c->second.size(); i++) {
					ofstream overfeat_out("overfeat.dat");
					CImg<double> subsampled = subsample(c->second[i], sample_size);
					string cmd = "./overfeat/bin/linux_64/overfeat -L 21 "+c->second[i]+" >> overfeat.dat";
					system(cmd.c_str());
					d_dim_vector = get_vector_of_features(sample_size);
					fileWriteMap.insert(make_pair(class_num, d_dim_vector));
				}
				++class_num;
			}
		}

	void overfeat_training(const Dataset &filenames){
		string filename = _algo + "_training.dat";
		fileWriteMap.clear();
		compute_overfeat(filenames);
		write_map_to_file(filename);
		execute_command("./svm_multiclass_learn -c 1.0 "+filename+" "+_algo+"_model.dat");
	}

	void overfeat_testing(const Dataset &filenames){
		string filename = _algo + "_testing.dat";
		fileWriteMap.clear();
		compute_overfeat(filenames);
		write_map_to_file(filename);
		execute_command("./svm_multiclass_classify "+filename+" "+_algo+"_model.dat"+" "+_algo+"_output.dat");
	}

};