template<class T>
class Eigen {
public:

	static const int subsample_size = 40;
	static const int top_k = 400;
	vector<CImg<double> > all_image_dump;
	multimap<int, vector<string> > fileWriteMap;
	CImg<double> eigenVector;
	CImg<double> eigenValues;
	CImg<double> input_eigen_orig;
 	CImg<double> input_eigen;
	CImg<double> eigen_image;
	CImg<double> subsampled;
	CImg<double> top_eigen;

	/*integer to string conversion*/
	string int_to_string(int val) {
		stringstream s;
		s << val;
		return s.str();
	}

	/*integer to string conversion*/
	string double_to_string(double val) {
		stringstream s;
		s << val;
		return s.str();
	}

	CImg<double> subsample(const string &filename, const int subsample_size) {
		return (CImg<double>(filename.c_str())).resize(subsample_size,
				subsample_size, 1, 1).unroll('x');
	}

	void convert_to_matrix() {
		int eigen_row = 0;
		CImg<double> dummy(subsample_size*subsample_size*3,all_image_dump.size(),1,1);
		input_eigen_orig = dummy;
		for (int i = 0; i < all_image_dump.size(); i++) {
			CImg<double> image = all_image_dump[i];
			for (int col = 0; col < image.width(); col++) {
				for (int row = 0; row < image.height(); row++) {
					input_eigen_orig(col, eigen_row) = image(col, row);
					input_eigen_orig(col, eigen_row) = image(col, row);
					input_eigen_orig(col, eigen_row) = image(col, row);
				}
			}
			++eigen_row;
		}
		cout << "New image width : " << input_eigen_orig.width() << ", height : "
				<< input_eigen_orig.height() << endl;
	}

	void convert_all_images_to_vector_space(const Dataset &filenames) {
		for (Dataset::const_iterator c = filenames.begin();
				c != filenames.end(); ++c) {
			for (int i = 0; i < c->second.size(); i++) {
				subsampled = subsample(c->second[i],subsample_size);
				all_image_dump.push_back(subsampled);
			}
		}
		cout << "all_image_dump size : " << all_image_dump.size() << endl;
	}

	CImg<double> calc_covariance() {
		/*mean calculation*/
		vector<double> meanVector;
		for (int i = 0; i < input_eigen_orig.width(); i++) {
			double sum = 0;
			for (int j = 0; j < input_eigen_orig.height(); j++) {
				sum += input_eigen_orig.atXY(j, i);
			}
			meanVector.push_back((double) sum / input_eigen_orig.width());
		}

		//Subtract mean
		for (int i = 0; i < input_eigen_orig.width(); i++) {
			for (int j = 0; j < input_eigen_orig.height(); j++) {
				input_eigen_orig.atXY(j, i) -= meanVector[i];
				input_eigen_orig.atXY(j, i)/input_eigen_orig.width();
			}
		}

		CImg<double> transpose(input_eigen_orig.height(), input_eigen_orig.width());
		//transpose
		for (int i = 0; i < input_eigen_orig.width(); i++) {
			for (int j = 0; j < input_eigen_orig.height(); j++) {
				transpose.atXY(j, i) = input_eigen_orig.atXY(i, j); //get the transpose of matrix
			}
		}
		cout<<"eigen : width : "<<input_eigen_orig.width()<<", height : "<<input_eigen_orig.height()<<endl;
		cout<<"transpose : width : "<<transpose.width()<<", height : "<<transpose.height()<<endl;
		cout<<"getting covariance : ";
		transpose*=input_eigen_orig;
		cout<<"covariance calculated: ";

		return transpose;
	}

	/*writes feature map to file*/
	void write_to_file(string filename) {
		vector<string> feature;
		int class_num=1, count=0;
		for(int i=0;i<input_eigen_orig.height();i++){
			feature.clear();
			for(int j=0;j<input_eigen_orig.width();j++){
				feature.push_back(int_to_string(j + 1) + ":" + double_to_string(input_eigen_orig.atXY(j,i)));
			}
			fileWriteMap.insert(make_pair(class_num, feature));
			if((i+1)%25==0) class_num++;
		}

		ofstream feature_file(filename.c_str());
		if (feature_file.is_open()) {
			multimap<int, vector<string> >::iterator map_iter;
			for (map_iter = fileWriteMap.begin();
					map_iter != fileWriteMap.end(); ++map_iter) {
				feature_file << map_iter->first <<" ";
				for (int i = 0; i < map_iter->second.size(); i++) {
					feature_file << map_iter->second[i] << " ";
				}
				feature_file << endl;
			}
			feature_file.close();
		}
		cout<<"File writing done!"<<endl;
	}

	void select_top_k_eigens(){
		cout<<"picking top eigen vectors: ";
		CImg<double> dummy_top(top_k, eigenVector.width());
		top_eigen = dummy_top;
		cout<<"top eigen : width"<<top_eigen.width()<<" height : "<<top_eigen.height()<<" eigenVector width "<<eigenVector.width()<<" height "<<eigenVector.height()<<" top k : "<<top_k<<endl;
		for(int i=0;i<top_k;i++){
			for(int j=0;j<eigenVector.height();j++){
				cout<<"i value: " << i << "j value :" <<j <<endl;
				top_eigen(i,j) = eigenVector.atXY(i,j);
				cout<< "the value inserted :" << top_eigen(i,j) << endl;
			}
		}
		cout<<"top eigen : width"<<top_eigen.width()<<" height : "<<top_eigen.height()<<" eigenVector width "<<eigenVector.width()<<" height "<<eigenVector.height()<<" top k : "<<top_k<<endl;
		cout<<"Finisahed picking top eigen vectors: ";		
	}

	void transform_to_eigenspace(){
		cout<<"Starting Eigen Decompose ";
		select_top_k_eigens();
		cout<<"output eigenspace: ";
		cout<<"eigen : width : "<<input_eigen_orig.width()<<", height : "<<input_eigen_orig.height()<<endl;
		cout<<"top eigen : width : "<<top_eigen.width()<<", height : "<<top_eigen.height()<<endl;

		CImg<double> transpose(top_eigen.height(),top_eigen.width(),1,3);
		for (int i = 0; i < top_eigen.width(); i++) {
			for (int j = 0; j < top_eigen.height(); j++) {
				transpose.atXY(j, i) = top_eigen.atXY(i, j); //get the transpose of matrix
			}
		}

		/*for (int k = 0; k < input_eigen_orig.width(); i++) {
		//	for (int l = 0; l < input_eigen_orig.height(); j++) {
				transpose_input_eigen_orig.atXY(l, k) = input_eigen_orig.atXY(k, l); //get the transpose of matrix
			}
		}*/

		cout<<"transpose top eigen : width : "<<transpose.width()<<", height : "<<transpose.height()<<endl;
		input_eigen_orig*=top_eigen;
		cout<<"eigen : width : "<<input_eigen_orig.width()<<", height : "<<input_eigen_orig.height()<<endl;
		cout<<"output eigenspace calculated: ";
	}

	void eigen_decomposition(string filename) {
		CImg<double> cov = calc_covariance();
		cout<<"covariance : width : "<<cov.width()<<" , height : "<<cov.height()<<endl;
		cov.symmetric_eigen(eigenValues,eigenVector);	
		transform_to_eigenspace();
		write_to_file(filename);
	}

	void execute_command(string command) {
		const char* command_str = command.c_str();
		system(command_str);
	}

	void eigen_training(const Dataset &filenames, string _algo) {	
		string filename = _algo + "_training.dat";
		convert_all_images_to_vector_space(filenames);
		convert_to_matrix();
		eigen_decomposition(filename);
		execute_command("./svm_multiclass_learn -c 1.0 "+filename+" "+_algo+"_model.dat");
	}

	void eigen_testing(const Dataset &filenames, string _algo) {
		string filename = _algo + "_training.dat";
		convert_all_images_to_vector_space(filenames);
		convert_to_matrix();
		eigen_decomposition(filename);
		execute_command("./svm_multiclass_classify "+filename+" "+_algo+"_model.dat"+" "+_algo+"_output.dat");
	}
	void call_train_test(string mode, const Dataset &filenames) {
		if (mode == "train" || mode == "tempTrain") {
			eigen_training(filenames, "eigen");
		} else if (mode == "test" || mode == "tempTest") {
			eigen_testing(filenames, "eigen");
		}
	}
};
