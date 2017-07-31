#include <math.h>
template<class T>
class KMeans {
public:

	static const int k_means = 100;
	vector<SiftDescriptor> centroids;
	multimap<int, vector<string> > fileWriteMap;
	static const int sift_length = 128;

	/*struct for storing sift features with corresponding centroids*/
	struct struct_sift {
		SiftDescriptor sift_feature;
		int cluster_id;
	}str_obj;
	vector<struct_sift> struct_sift_vector;

	/*integer to string conversion*/
	string int_to_string(int val) {
		stringstream s;
		s << val;
		return s.str();
	}

	/*euclidean distance calculation*/
	double distance(SiftDescriptor first, SiftDescriptor second) {
		double distanceVal = 0,totalf=0,totals=0,stddevf=0,stddevs=0;
		for(int i=0;i<128;i++){
			totalf+=first.descriptor[i];
			totals+=second.descriptor[i];
		}
		for(int i=0;i<128;i++){
			stddevf+=((first.descriptor[i]-totalf)*(first.descriptor[i]-totalf));
			first.descriptor[i]-=totalf;
			stddevs+=((second.descriptor[i]-totals)*(second.descriptor[i]-totals));
			second.descriptor[i]-=totals;		
		}
		stddevf=sqrt(stddevf/128);
		stddevs=sqrt(stddevs/128);
		for (int i = 0; i < 128; i++) {
			first.descriptor[i]=(first.descriptor[i]/stddevf)*1000;
			second.descriptor[i]=(second.descriptor[i]/stddevs)*1000;
			//cout<<"first : "<<first.descriptor[i]<<" second : "<<second.descriptor[i]<<endl;
			distanceVal = distanceVal
					+ pow(first.descriptor[i] - second.descriptor[i], 2);
			if(distanceVal!=distanceVal){
				//cout<<first.descriptor[i]<<" " <<second.descriptor[i]<<endl;
			}
		}
		//cout << "distance : " << distanceVal << endl;
		
		return distanceVal;
	}
	void execute_command(string command) {
		const char* command_str = command.c_str();
		system(command_str);
	}

	/*get all sift vectors of all images (training/testing)*/
	vector<SiftDescriptor> get_all_sift_vectors(const Dataset &filenames) {
		vector < SiftDescriptor > all_sift_desc;
		for (Dataset::const_iterator c = filenames.begin();
				c != filenames.end(); ++c) {
			for (int i = 0; i < c->second.size(); i++) {
				CImg<double> input_image(c->second[i].c_str());
				CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
				vector < SiftDescriptor > descriptors = Sift::compute_sift(
						gray);
				all_sift_desc.insert(all_sift_desc.end(), descriptors.begin(),
						descriptors.end());
			}
		}
		//cout << "all_sift_desc.size() : " << all_sift_desc.size() << endl;
		return all_sift_desc;
	}

	/*random initial centroids*/
	void get_initial_centroids(vector<SiftDescriptor> all_sift_desc) {
		//cout << "getting initial centroids" << endl;
		for (int i = 0; i < k_means; i++) {
			srand(1234);
			int rand_index = rand() % (all_sift_desc.size() - 1) + 1;
			//cout << "index : " << rand_index << endl;
			centroids.push_back(all_sift_desc[rand_index]);
		}
		//cout << "\n initial centroids received : " << centroids.size() << endl;
	}

	/*dumps all the sift features to struct with initial centroids assigned as 0*/
	void dump_all_features_to_struct(vector<SiftDescriptor> all_sift) {
		//cout << "dumping all features to struct" << all_sift.size()<<endl;
		for (int i = 0; i < all_sift.size(); i++) {
			str_obj.sift_feature = (SiftDescriptor)all_sift[i];
			str_obj.cluster_id = 0;
			struct_sift_vector.push_back(str_obj);
			//cout<<"dump_all_features_to_struct | Cluster id : "<<struct_sift_vector[i].cluster_id<<endl;
		}
		//cout << "dumping all features to struct done : "<< struct_sift_vector.size() << endl;
	}

	/*clusters the sift vectors to corresponding bins as per the least distance between the vectors*/
	void cluster() {
		int min_dist, min_index;
		for (int i = 0; i < struct_sift_vector.size(); i++) {
			str_obj = struct_sift_vector[i];
			min_dist = 999999;
			min_index = 0;
			//cout<<"for feature "<<i<<" : distances : "<<endl;
			for (int j = 0; j < centroids.size(); j++) {
				double act_dist = distance(str_obj.sift_feature, centroids[j]);
				//cout<<""<<(j+1)<<" : "<<act_dist<<" ";
				if (act_dist < min_dist) {
					min_dist = act_dist;
					min_index = j;
				}
			}
			//cout<<"\nact_dist found : "<<min_dist<<endl;
			str_obj.cluster_id = min_index + 1;
			struct_sift_vector[i] = str_obj;
		}
	}

	void print_cluster_ids(){
		for(int i=0;i<centroids.size();i++){
			//cout<<struct_sift_vector[i].cluster_id<<" ";
			SiftDescriptor sift = centroids[i];
			//cout << "Cluster : "<<(i+1)<<"(";
			for(int k=0;k<128;k++){
				//cout<<sift.descriptor[k]<<",";	
			}
			cout<<")"<<endl;
		}
	}

	/*reiteration for getting new k centroids*/
	void recalculate_cluster_centers() {
		//cout<<"recalculating cluster centers"<<endl;
		vector < SiftDescriptor > sift_per_cluster;
		vector<double> mean_vector(128);
		for (int i = 1; i <= k_means; i++) {
			sift_per_cluster.clear();
			for (int j = 0; j < struct_sift_vector.size(); j++) {
				if (struct_sift_vector[j].cluster_id == i) {
					sift_per_cluster.push_back(
							struct_sift_vector[j].sift_feature);
				}
			}
			//cout << "recalculate :: sift_feature of same cluster pushed together"<<endl;
			mean_vector.clear();
			mean_vector.resize(sift_length);
			for (int j = 0; j < mean_vector.size(); j++) {
				for (int l = 0; l < sift_per_cluster.size(); l++) {
					mean_vector[j] += sift_per_cluster[l].descriptor[j];
				}
			}
			//cout << "recalculate : mean_vectors updated"<<endl;
			for (int j = 0; j < mean_vector.size(); j++) {
				mean_vector[j] = mean_vector[j] / sift_per_cluster.size();
				centroids[i - 1].descriptor[j] = mean_vector[j];
			}
			//cout << "recalculate : mean recalculated"<<endl;
		}
		//cout<<"recalculating cluster centers done"<<endl;
	}

	/*histogram over centroids feature extraction*/
	vector<string> hist_each_sift(vector<SiftDescriptor> sift_features) {
		//cout << "histogram of each sift started";
		vector<int> poll_count(k_means);
		vector < string > feature;
		double min_dist = 99999999;
		for (int i = 0; i < sift_features.size(); i++) {
			min_dist = 99999999;
			for (int j = 0; j < centroids.size(); j++) {
				double act_dist = distance(sift_features[i], centroids[j]);
				if (act_dist < min_dist) {
					min_dist = act_dist;
					poll_count[j] = poll_count[j] + 1;
				}
			}
		}
		//cout << "Poll count :";
		for (int i = 0; i < poll_count.size(); i++) {
			feature.push_back(
					int_to_string(i + 1) + ":" + int_to_string(poll_count[i]));
			//cout << poll_count[i] << " ";
		}
		//cout << "\nhistogram of each sift ended"<<endl;
		return feature;
	}

	/*actual histogram creation over k centroids*/
	void create_histogram_over_centroids(const Dataset &filenames) {
		cout << "created histogram over centroids started";
		int class_num = 1;
		vector < string > each_img_feature;
		for (Dataset::const_iterator c = filenames.begin();
				c != filenames.end(); ++c) {
			for (int i = 0; i < c->second.size(); i++) {
				CImg<double> input_image(c->second[i].c_str());
				CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
				vector < SiftDescriptor > descriptors = Sift::compute_sift(
						gray);
				vector < string > result = hist_each_sift(descriptors);
				each_img_feature.insert(each_img_feature.end(), result.begin(),
						result.end());
				fileWriteMap.insert(make_pair(class_num, each_img_feature));
				each_img_feature.clear();
			}
			++class_num;
		}
		cout << "created histogram over centroids ended";
	}

	/*writes feature map to file*/
	void write_map_to_file(string filename, string _algo) {
		cout << "KMeans | write_map_to_file :: info : inside function" << endl;

		ofstream feature_file(filename.c_str());
		if (feature_file.is_open()) {
			//remove(filename.c_str());
			feature_file << "# Feature file for " << _algo << endl;
			multimap<int, vector<string> >::iterator map_iter;
			for (map_iter = fileWriteMap.begin();
					map_iter != fileWriteMap.end(); ++map_iter) {
				feature_file << map_iter->first << " ";
				for (int i = 0; i < map_iter->second.size(); i++) {
					feature_file << map_iter->second[i] << " ";
				}
				feature_file << endl;
			}
			feature_file.close();
		}
		cout << "File writing done!" << endl;
	}

	/*k-means function call*/
	void k_means_algo_training(const Dataset &filenames, string _algo) {
		fileWriteMap.clear();
		string filename = _algo + "_training.dat";
		vector < SiftDescriptor > all_sift_vect = get_all_sift_vectors(
				filenames);
		get_initial_centroids (all_sift_vect);
		dump_all_features_to_struct(all_sift_vect);
		for (int i = 0; i < 10000; i++) {
			cluster();
			//print_cluster_ids();
			recalculate_cluster_centers();
		}
		create_histogram_over_centroids(filenames);
		write_map_to_file(filename, _algo);
		execute_command(
				"./svm_multiclass_learn -c 1.0 " + filename + " " + _algo
						+ "_model.dat");
	}

	void k_means_algo_testing(const Dataset &filenames, string _algo) {
		fileWriteMap.clear();
		string filename = _algo + "_testing.dat";
		vector < SiftDescriptor > all_sift_vect = get_all_sift_vectors(
				filenames);
		get_initial_centroids (all_sift_vect);
		dump_all_features_to_struct(all_sift_vect);
		for (int num = 0; num < 10000; num++) {
			cluster();
			recalculate_cluster_centers();
		}
		create_histogram_over_centroids(filenames);
		write_map_to_file(filename, _algo);
		execute_command(
				"./svm_multiclass_classify " + filename + " " + _algo
						+ "_model.dat" + " " + _algo + "_output.dat");
	}

	void call_train_test(string mode, const Dataset &filenames) {
		if (mode == "train") {
			k_means_algo_training(filenames, "bow");
		} else if (mode == "test") {
			k_means_algo_testing(filenames, "bow");
		}
	}

};