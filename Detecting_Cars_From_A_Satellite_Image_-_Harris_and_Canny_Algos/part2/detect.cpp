//
// detect.cpp : Detect cars in satellite images.
//
// Based on skeleton code by D. Crandall, Spring 2017
//
// Naren Suri -nsuri
//Rohit Gattani - rgattani
//Siddhartha Pagadala - sidpagad

#include <SImage.h>
#include <SImageIO.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#define PI 3.14159265
#define INF 9999

using namespace std;

 /*Threshold values
 *     Variables
 *         User Changeable
 *           */

int canny_threshold = 35;
float match_confidence_threshold = 0.1;
int multiple_detection_nearest_threshold = 2500;
// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane will be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below is a helper functions that overlays rectangles
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
  for(int w=-width/2; w<=width/2; w++) {
    int top = _top+w, left = _left+w, right=_right+w, bottom=_bottom+w;

    // if any of the coordinates are out-of-bounds, truncate them 
    top = min( max( top, 0 ), input.rows()-1);
    bottom = min( max( bottom, 0 ), input.rows()-1);
    left = min( max( left, 0 ), input.cols()-1);
    right = min( max( right, 0 ), input.cols()-1);
      
    // draw top and bottom lines
    for(int j=left; j<=right; j++)
	  input[top][j] = input[bottom][j] = graylevel;
    // draw left and right lines
    for(int i=top; i<=bottom; i++)
	  input[i][left] = input[i][right] = graylevel;
  }
}

// DetectedBox class may be helpful!
//  Feel free to modify.
//
class DetectedBox {
public:
  int row, col, width, height;
  double confidence;
};

// Function that outputs the ascii detection output file
void  write_detection_txt(const string &filename, const vector<DetectedBox> &cars)
{
  ofstream ofs(filename.c_str());

  for(vector<DetectedBox>::const_iterator s=cars.begin(); s != cars.end(); ++s)
    ofs << s->row << " " << s->col << " " << s->width << " " << s->height << " " << s->confidence << endl;
}

// Function that outputs a visualization of detected boxes
void  write_detection_image(const string &filename, const vector<DetectedBox> &cars, const SDoublePlane &input)
{
  SDoublePlane output_planes[3];

  for(int p=0; p<3; p++)
    {
      output_planes[p] = input;
      for(vector<DetectedBox>::const_iterator s=cars.begin(); s != cars.end(); ++s)
	overlay_rectangle(output_planes[p], s->row, s->col, s->row+s->height-1, s->col+s->width-1, p==2?255:0, 2);
    }

  SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);
}


int reflect_edges_index(int N, int x){
   if(x < 0){
      return -x - 1;
   }
   if(x>=N){
      return 2*N-x-1;
   }
   return x;
}
// The rest of these functions are incomplete. These are just suggestions to 
// get you started -- feel free to add extra functions, change function
// parameters, etc.

double convolve_separable_general_single_pix_reflective(const SDoublePlane &input, const SDoublePlane &filter, int x, int y,bool horizontal){
   double sum = 0;
   int m = filter.rows();
   
   for(int i=-(m/2);i<((m/2)+1);i++){
	   int x_updated = reflect_edges_index(input.rows(),x-i);
   	   int y_updated = reflect_edges_index(input.cols(),y-i);
        if(horizontal){
           sum = sum + (filter[0][i+(m/2)] * input[reflect_edges_index(input.rows(),x)][y_updated]);
        }else{
	   sum = sum + (filter[i+(m/2)][0] * input[x_updated][reflect_edges_index(input.cols(),y)]);
	}
   }
   return sum;

}

SDoublePlane convolve_separable_reflective(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane temp_output(input.rows(), input.cols());

  
  for(int j=0; j<input.cols();j++){
        for(int i=0; i<input.rows();i++){
           temp_output[i][j] = convolve_separable_general_single_pix_reflective(input, col_filter, i, j,false);
        }
  }

   for(int i=0; i<input.rows();i++){
        for(int j=0; j<input.cols();j++){
           output[i][j] = convolve_separable_general_single_pix_reflective(temp_output, row_filter, i, j,true);
        }
  }

  return output;
}

double convolve_separable_general_single_pix(const SDoublePlane &input, const SDoublePlane &filter, int x, int y,bool horizontal){
   double sum = 0;
   int m = filter.rows();
   
   for(int i=-(m/2);i<((m/2)+1);i++){
        if(horizontal){
           sum = sum + (filter[0][i+(m/2)] * input[x][y-i]);
        }else{
	   sum = sum + (filter[i+(m/2)][0] * input[x-i][y]);
	}
   }
   return sum;

}

// Convolve an image with a separable convolution kernel
//
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane temp_output(input.rows(), input.cols());

  // Convolution code here
   for(int i=0;i<input.rows();i++){
        for(int j=0; j<input.cols();j++){
           output[i][j]=0;
	   temp_output[i][j] = 0;
        }
  }
  // Convolution code here
  
  for(int j=0; j<input.cols();j++){
        for(int i=1; i<input.rows()-1;i++){
           temp_output[i][j] = convolve_separable_general_single_pix(input, col_filter, i, j,false);
        }
  }

   for(int i=0; i<input.rows();i++){
        for(int j=1; j<input.cols()-1;j++){
           output[i][j] = convolve_separable_general_single_pix(temp_output, row_filter, i, j,true);
        }
  }

  return output;
}

double convolve_general_single_pix(const SDoublePlane &input, const SDoublePlane &filter, int x, int y){
   double sum = 0;
   int m = filter.rows();
   int n = filter.cols();
   for(int i=-(m/2);i<((m/2)+1);i++){
   	for(int j=-(n/2);j<((n/2)+1);j++){
	   sum = sum + (filter[i+(m/2)][j+(n/2)] * input[x-i][y-j]);
	}
   }
   return sum;

}


double convolve_general_single_pix_reflective(const SDoublePlane &input, const SDoublePlane &filter, int x, int y){
   double sum = 0;
   int m = filter.rows();
   int n = filter.cols();
   for(int i=-(m/2);i<((m/2)+1);i++){
        for(int j=-(n/2);j<((n/2)+1);j++){
   	   int x_updated = reflect_edges_index(input.rows(),x-i);
   	   int y_updated = reflect_edges_index(input.cols(),y-j);
           sum = sum + (filter[i+(m/2)][j+(n/2)] * input[x_updated][y_updated]);
        }
   }
   return sum;
}

SDoublePlane convolve_general_reflective(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output(input.rows(), input.cols());
 
  // Convolution code here
   for(int i=0; i<input.rows();i++){
   	for(int j=0; j<input.cols();j++){
           output[i][j] = convolve_general_single_pix_reflective(input, filter, i, j);
        }
    }
    return output;
 }
  //
int replication_edges_index(int N, int x){
   if(x < 0){
      return x + N;
   }
   if(x>=N){
      return x-N;
   }
   return x;
}

double convolve_general_single_pix_replication(const SDoublePlane &input, const SDoublePlane &filter, int x, int y){
   double sum = 0;
   int m = filter.rows();
   int n = filter.cols();
   for(int i=-(m/2);i<((m/2)+1);i++){
        for(int j=-(n/2);j<((n/2)+1);j++){
           int x_updated = replication_edges_index(input.rows(),x-i);
           int y_updated = replication_edges_index(input.cols(),y-j);
           sum = sum + (filter[i+(m/2)][j+(n/2)] * input[x_updated][y_updated]);
        }
   }
   return sum;
}

SDoublePlane convolve_general_replication(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output(input.rows(), input.cols());

  // Convolution code here
   for(int i=0; i<input.rows();i++){
      for(int j=0; j<input.cols();j++){
         output[i][j] = convolve_general_single_pix_replication(input, filter, i, j);
      }
  }
  return output;
}
  //
  
// Convolve an image with a  convolution kernel
//
SDoublePlane convolve_general_no_border(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output(input.rows(), input.cols());
  for(int i=0;i<input.rows();i++){
	for(int j=0; j<input.cols();j++){
	   output[i][j]=0;
	}
  }
  // Convolution code here
  for(int i=1; i<input.rows()-1;i++){
	for(int j=1; j<input.cols()-1;j++){
	   output[i][j] = convolve_general_single_pix(input, filter, i, j);
	}
  }
  return output;
}


// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
  SDoublePlane output(input.rows(), input.cols());
  
  SDoublePlane s_x(3,3);
  
  s_x[0][0] = -1/8.0;
  s_x[0][1] = 0;
  s_x[0][2] = 1/8.0;
  s_x[1][0] = -2/8.0;
  s_x[1][1] = 0;
  s_x[1][2] = 2/8.0;
  s_x[2][0] = -1/8.0;
  s_x[2][1] = 0;
  s_x[2][2] = 1/8.0;
  
  SDoublePlane s_y(3,3);
  
  s_y[0][0] = 1/8.0;
  s_y[0][1] = 2/8.0;
  s_y[0][2] = 1/8.0;
  s_y[1][0] = 0;
  s_y[1][1] = 0;
  s_y[1][2] = 0;
  s_y[2][0] = -1/8.0;
  s_y[2][1] = -2/8.0;
  s_y[2][2] = -1/8.0;
  
  if(_gx){
	  output = convolve_general_reflective(input, s_x);
  }else{
	  output = convolve_general_reflective(input, s_y);
  }
  // Implement a sobel gradient estimation filter with 1-d filters
  

  return output;
}

int convert_angles(double x){
	if(0 <= x && x< 22.5){
		return 0;
	}else if(22.5 <= x && x< 45){
		return 45;
	}else if(45 <= x && x< 67.5){
		return 45;
	}else if(67.5 <= x && x<= 90){
		return 90;
	}else if(-22.5<= x && x<0){
		return 0;
	}else if(-45 <= x && x<= 22.5){
		return 135;
	}else if(-67.5 <= x && x< -45){
		return 135;
	}else if(-90 <= x && x< -67.5){
		return 90;
	}
}

// Apply an edge detector to an image, returns the binary edge map
// 
SDoublePlane find_edges(const SDoublePlane &input, double threshold=0)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane gradient_magnitude(input.rows(), input.cols());
  SDoublePlane gradient_orientation(input.rows(), input.cols());  

  SDoublePlane gradient_x(input.rows(), input.cols());
  SDoublePlane gradient_y(input.rows(), input.cols());
  SDoublePlane non_max_suppression_gradient(input.rows(), input.cols());
  
  gradient_x = sobel_gradient_filter(input,true);
  gradient_y = sobel_gradient_filter(input,false);
  
  int angle_disc;  
  double gradient_ratio, angle;

  for(int i=0;i<input.rows();i++){
	  for(int j=0;j<input.cols();j++){
		gradient_magnitude[i][j] = sqrt(pow(gradient_x[i][j],2)+pow(gradient_y[i][j],2));
		  gradient_ratio = gradient_y[i][j]/gradient_x[i][j];
		  angle = atan (gradient_ratio) * 180 / PI;
		  angle_disc = convert_angles(angle);
		  //angle = atan2 (gradient_y[i][j],gradient_x[i][j]) * 180 / PI;
		  
		  gradient_orientation[i][j] = angle_disc;
		 // output[i][j] = sqrt(pow(gradient_x[i][j],2)+pow(gradient_y[i][j],2));
	  }
  }

  for(int i=0;i<input.rows();i++){
	  for(int j=0;j<input.cols();j++){
		  angle = gradient_orientation[i][j];
			if(angle == 0){
				if(j > 0 && j < input.cols()-1){
					if(gradient_magnitude[i][j-1] < gradient_magnitude[i][j] && gradient_magnitude[i][j] > gradient_magnitude[i][j+1] &&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}else if(j == 0 ){
					if(gradient_magnitude[i][j] > gradient_magnitude[i][j+1] &&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}else if(j == input.cols()-1){
					if(gradient_magnitude[i][j-1] < gradient_magnitude[i][j] &&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}
				
			}else if(angle == 90){
				if(i > 0 && i < input.rows()-1){
					if(gradient_magnitude[i-1][j] < gradient_magnitude[i][j] && gradient_magnitude[i][j] > gradient_magnitude[i+1][j] &&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}else if(i == 0 ){
					if(gradient_magnitude[i][j] > gradient_magnitude[i+1][j]&&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}else if(i == input.rows()-1){
					if(gradient_magnitude[i-1][j] < gradient_magnitude[i][j]&&  gradient_magnitude[i][j] >= threshold){
						non_max_suppression_gradient[i][j] = 255;
					}else{
						non_max_suppression_gradient[i][j] = 0;
					}
				}
			}
	  else if(angle == 45){
		if(i >0 && j >0 && i < input.rows()-1 && j < input.cols()-1){
			if(gradient_magnitude[i-1][j+1] < gradient_magnitude[i][j] && gradient_magnitude[i][j] > gradient_magnitude[i+1][j-1]&&  gradient_magnitude[i][j] >= threshold){
				non_max_suppression_gradient[i][j] = 255;
			}else{
				non_max_suppression_gradient[i][j] = 0;
			}
		}
	}else if(angle == 135){
		if(i >0 && j >0 && i < input.rows()-1 && j < input.cols()-1){
			if(gradient_magnitude[i-1][j-1] < gradient_magnitude[i][j] && gradient_magnitude[i][j] > gradient_magnitude[i+1][j+1]&&  gradient_magnitude[i][j] >= threshold){
				non_max_suppression_gradient[i][j] = 255;
			}else{
				non_max_suppression_gradient[i][j] = 0;
			}
		}
	}
    }
 }
 
  //SImageIO::write_png_file("gradient_non_max_suppression.png",non_max_suppression_gradient,non_max_suppression_gradient,non_max_suppression_gradient);
  // Implement an edge detector of your choice, e.g.
  // use your sobel gradient operator to compute the gradient magnitude and threshold
 // SImageIO::write_png_file("sobel_gradient_magnitude.png",gradient_magnitude,gradient_magnitude,gradient_magnitude);
  //SImageIO::write_png_file("sobel_gradient_orientation.png",gradient_orientation,gradient_orientation,gradient_orientation);
  
  output = non_max_suppression_gradient;
  return non_max_suppression_gradient;
}

/*
	Functionality : Crops template car from input binary image(Edge map)
	Input : SDoublePlane input_binary_image
	Output : SDoublePlane templeta_binary_image
*/
void get_template(const SDoublePlane &non_max_suppression_gradient){
	cout << "Inside get_template function" << endl;
	int template_start_row, template_start_col;
	template_start_row = 450;
	template_start_col = 200;
	SDoublePlane template_test(49, 24);  
	for(int i=0; i<49;i++){
		for(int j=0; j<24; j++){
			template_test[i][j] = non_max_suppression_gradient[template_start_row+i][template_start_col+j];
		}
	}
	
	SImageIO::write_png_file("template_car.png",template_test,template_test,template_test);
	
	//return template_test;
}

SDoublePlane non_max_suppression_match_score(SDoublePlane map_match){
	float thres = 0.001;
	for(int i=1;i<map_match.rows()-1;i++){
			for(int j=1; j<map_match.cols()-1;j++){
				//north check
				if((map_match[i][j] - map_match[i-1][j]) > thres){
					map_match[i][j] = INF;
				}
				//south check
				if((map_match[i][j] - map_match[i+1][j]) > thres){
					map_match[i][j] = INF;
				}
				//east check
				if((map_match[i][j] - map_match[i][j+1]) > thres){
					map_match[i][j] = INF;
				}
				//west check
				if((map_match[i][j] - map_match[i][j-1]) > thres){
					map_match[i][j] = INF;
				}
				//north-east check
				if((map_match[i][j] - map_match[i-1][j+1]) > thres){
					map_match[i][j] = INF;
				}
				//north-west check
				if((map_match[i][j] - map_match[i-1][j-1]) > thres){
					map_match[i][j] = INF;
				}
				//south-east check
				if((map_match[i][j] - map_match[i+1][j+1]) > thres){
					map_match[i][j] = INF;
				}
				//south-west check
				if((map_match[i][j] - map_match[i+1][j-1]) > thres){
					map_match[i][j] = INF;
				}
			}
	}
	return map_match;
}

/*
	Functionality : Calculates distance transform values of input binary image
	Input : SDoublePlane input_binary_image
	Output : SDoublePlane distance_transformed_values
*/
SDoublePlane distance_transform(const SDoublePlane &input, float &foreground_pix_num){
	
	//Initialization
	//Matrix with distance transformed values of binary input image
	//size: input_image_rows x input_image_cols
	SDoublePlane distance(input.rows(), input.cols());
	
	/*Definition of distance masks
	  Euclidean Distance with integral values
	*/
	
	//First traversal mask
	int m_1_f = 3;
	int m_2_f = 4;
	int m_3_f = 3;
	int m_4_f = 4;
	
	//second traversal mask
	int m_1_s = 3;
	int m_2_s = 4;
	int m_3_s = 3;
	int m_4_s = 4;
	
		
	int d_1,d_2,d_3,d_4; 
	//Initializes distances with 0 for foreground pixels(with 1 value) and infinity for background pixels(with 0 value)
	for(int i=0; i<input.rows() ; i++){
		for(int j=0; j<input.cols() ; j++){
			if(input[i][j] == 255){
				//foreground pixel
				foreground_pix_num = foreground_pix_num +1;
				distance[i][j] = 0; 
			}else{
				//background pixel
				distance[i][j] = INF;
			}
		}
	}
	
	//Top left corner to bottom right corner traversal of image
	//top to bottom traversal
	for(int j=1; j<input.cols(); j++ ){
		//left to right traversal
		for(int i=1; i<input.rows()-1; i++){
			d_1,d_2,d_3, d_4 = 0;
			if(distance[i][j] > 0){
				d_1 = m_1_f + distance[i-1][j];
				d_2 = m_2_f + distance[i-1][j-1];
				d_3 = m_3_f + distance[i][j-1];
				d_4 = m_4_f + distance[i+1][j-1];
				//distance[i][j] = std::min({d_1, d_2, d_3, d_4});
				distance[i][j] = std::min(d_1, std::min(d_2, std::min(d_3, d_4)));
			}
		}
	}
	
	//Bottom right corner to top left corner
	//bottom to top traversal
	for(int j=input.cols()-2;j>=0; j--){
		//right to left traversal
		for(int i=input.rows()-2;i>0;i--){
			d_1,d_2,d_3, d_4 = 0;
			if(distance[i][j] > 0 ){
				d_1 = m_1_s + distance[i+1][j];
				d_2 = m_2_s + distance[i+1][j+1];
				d_3 = m_3_s + distance[i][j+1];
				d_4 = m_4_s + distance[i-1][j+1];
				//distance[i][j] = std::min({d_1, d_2, d_3, d_4});
				distance[i][j] = std::min(d_1, std::min(d_2, std::min(d_3, d_4)));
			}
		}
	}
	
	return distance;
}

/*
	Functionality : Calculates chamfer match score --> Uses distance transform
					to localize the points of maximum agreement between binary 
					template and binary input image
	Input : SDoublePlane input_binary_image, SDoublePlane template_binary_image
	Output : SDoublePlane chamfer match score
*/
SDoublePlane chamfer_match(const SDoublePlane &input, vector<SDoublePlane> templates, float &min, float &second_min){
	
	//Initialization
	float k =0;	//number of foreground pixels in image
	//Matrix with distance transformed values of binary input image
	//size: input_image_rows x input_image_cols
	SDoublePlane distance = distance_transform(input, k);
	
	SDoublePlane template_image = templates.back();
	
				
	//float min = INF;
	//float second_min = INF;
	//Matrix with matching score
	//size: input_image_rows-template_rows+1 x input_image_cols-template_cols+1
	SDoublePlane map_match(input.rows()-template_image.rows()+1, input.cols()-template_image.cols()+1);
	for(int r=0; r<input.rows()-template_image.rows()+1 ; r++){
		for(int s=0; s<input.cols()-template_image.cols()+1 ; s++){
			map_match[r][s] = INF;
		}
	}
	
	while(!templates.empty()){
		template_image = templates.back();
		templates.pop_back();
		for(int r=0; r<input.rows()-template_image.rows()+1 ; r++){
			for(int s=0; s<input.cols()-template_image.cols()+1 ; s++){
				int q = 0;
				for(int i=0; i< template_image.rows(); i++){
					for(int j=0; j<template_image.cols(); j++){
						if(template_image[i][j] == 255){
							q = q + distance[r+i][s+j];
						}
					}
				}
				float temp = q/k;
				if(temp < map_match[r][s]){
					map_match[r][s] = temp;
					
					if(map_match[r][s] < min){
						second_min = min;
						min = map_match[r][s];
					}else if(map_match[r][s] < second_min){
						second_min = map_match[r][s];
					}
				}
				
			}
		}
	}
	return map_match;
}

/*
	Functionality : searches and detects the template_binary_image with in 
					input_binary_image(edge map) and mathches those below the given
					threshold
	Input : SDoublePlane input_binary_image, SDoublePlane template_binary_image, threshold
	
*/
vector<DetectedBox> detect_objects(const SDoublePlane &input, vector<SDoublePlane> templates, float threshold){
	
	vector<DetectedBox> cars;
	
	float min = INF;
	float second_min = INF;
	
	SDoublePlane chamfer_match_score_1 = chamfer_match(input, templates, min, second_min);
	SDoublePlane chamfer_match_score = non_max_suppression_match_score(chamfer_match_score_1);
	
	if(second_min == 0){
		threshold = 0.012;
	}else{

		threshold = 3*(second_min+0.001-min);
	}
	for(int r=0; r<chamfer_match_score.rows(); r++){
		for(int s=0; s<chamfer_match_score.cols(); s++){
			DetectedBox d;
			DetectedBox last_element;
			d.height = -1;
			if(chamfer_match_score[r][s] <= threshold){
				
				
				SDoublePlane template_image = templates.back();
				
				
				d.row = r;
				d.col = s;
				d.width = template_image.cols();
				d.height = template_image.rows();
				d.confidence = 1-chamfer_match_score[r][s];
				s = s + d.width;
				
				
				if(!cars.empty()){
					last_element = cars.back();
					
					if((pow(last_element.row-r,2)+pow(last_element.col-s,2))<=multiple_detection_nearest_threshold){
						if(last_element.confidence < d.confidence){
							cars.pop_back();
							cars.push_back(d);
						}
					}else{
						cars.push_back(d);
					}
				}else{
					cars.push_back(d);
				}
				

				//cout << "r:s:" <<  r << ":" << s << ":Chamfer Match Score:" << chamfer_match_score[r][s]<<endl;
				//r = r + template_image.cols();
				//s = s + template_image.rows();
			}
			
			
		}
	}
	return cars;
}


//
// This main file just outputs a few test images. You'll want to change it to do 
//  something more interesting!
//
int main(int argc, char *argv[])
{


  if(!(argc == 2))
    {
      cerr << "usage: " << argv[0] << " input_image" << endl;
      return 1;
    }

  string input_filename(argv[1]);
  SDoublePlane input_image= SImageIO::read_png_file(input_filename.c_str());
  // test step 2 by applying mean filters to the input image
  SDoublePlane mean_filter(3,3);
  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      mean_filter[i][j] = 1/9.0;
  SDoublePlane mean_filter_horizontal(1,3);
  SDoublePlane mean_filter_vertical(3,1);

   for(int i=0; i<3; i++){
	mean_filter_horizontal[0][i]=1/3.0;
	mean_filter_vertical[i][0]=1/1.0;
   }

/*
  SDoublePlane output_image = convolve_general_no_border(input_image, mean_filter);
  SImageIO::write_png_file("convolved_image_no_border.png",output_image,output_image,output_image);
  
  SDoublePlane output_image_reflective = convolve_general_reflective(input_image, mean_filter);
  SImageIO::write_png_file("convolved_image_reflective.png",output_image_reflective,output_image_reflective,output_image_reflective);
  
  SDoublePlane output_image_replication = convolve_general_replication(input_image, mean_filter);
  SImageIO::write_png_file("convolved_image_replication.png",output_image_replication,output_image_replication,output_image_replication);

  SDoublePlane output_image_linear_separable = convolve_separable_reflective(input_image, mean_filter_horizontal, mean_filter_vertical);
  SImageIO::write_png_file("convolved_image_separable.png",output_image_linear_separable,output_image_linear_separable,output_image_linear_separable);
  
  cout << "Normal:" << output_image[20][20] << endl;
  cout << "Reflective:" << output_image_reflective[20][20] << endl;
  cout << "Replication:" << output_image_replication[20][20] << endl;
  cout << "Linear Separable:" << output_image_linear_separable[20][20] << endl;
*/

  SDoublePlane sobel_image_result_x = sobel_gradient_filter(input_image,true);
  SDoublePlane sobel_image_result_y = sobel_gradient_filter(input_image,false);

/*
  SImageIO::write_png_file("sobel_x_gradient.png",sobel_image_result_x,sobel_image_result_x,sobel_image_result_x);
  SImageIO::write_png_file("sobel_y_gradient.png",sobel_image_result_y,sobel_image_result_y,sobel_image_result_y);
*/  
  SDoublePlane edge_input_image = find_edges(input_image,canny_threshold);
  SImageIO::write_png_file("edges.png",edge_input_image,edge_input_image,edge_input_image);
 
  SDoublePlane template_image_1= SImageIO::read_png_file("template_1.png"); 
  SDoublePlane template_image_2= SImageIO::read_png_file("template_2.png"); 
  SDoublePlane template_image_3= SImageIO::read_png_file("template_3.png"); 
  SDoublePlane template_image_4= SImageIO::read_png_file("template_4.png"); 
  
  vector<SDoublePlane> templates;
  
  templates.push_back(template_image_1);
  templates.push_back(template_image_2);
  templates.push_back(template_image_3);
  templates.push_back(template_image_4);
  
  
  
  //get_template(edge_input_image);
  
  
  vector<DetectedBox> cars = detect_objects(edge_input_image, templates,match_confidence_threshold );
  
  
  // randomly generate some detected cars -- you'll want to replace this
  //  with your car detection code obviously!
  /*
  vector<DetectedBox> cars;
  for(int i=0; i<10; i++)
    {
      DetectedBox s;
      s.row = rand() % input_image.rows();
      s.col = rand() % input_image.cols();
      s.width = 20;
      s.height = 20;
      s.confidence = rand();
      cars.push_back(s);
    }
	*/
  write_detection_txt("detected.txt", cars);
  write_detection_image("detected.png", cars, input_image);
}
