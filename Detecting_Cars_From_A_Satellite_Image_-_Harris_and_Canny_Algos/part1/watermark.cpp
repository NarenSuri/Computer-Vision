//
// Watermark.cpp : Add watermark to an image, or inspect if a watermark is present.
//
// Based on skeleton code by D. Crandall, Spring 2017
//
// Siddhartha Pagadala, Naren Suri, Rohit Gattani
// (Report at the end of file)
//

/* Reference - Discrete Fourier transform–based watermarking method with an optimal implementation radius
  Ante Poljicak, Lidija Mandic, Darko Agic
*/

//Link to the header file
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include "SImage.h"
#include "SImageIO.h"
#include "fft.h"

using namespace std;

// This code requires that input be a *square* image, and that each dimension
//  is a power of 2; i.e. that input.width() == input.height() == 2^k, where k
//  is an integer. You'll need to pad out your image (with 0's) first if it's
//  not a square image to begin with. (Padding with 0's has no effect on the FT!)
//
// Forward FFT transform: take input image, and return real and imaginary parts.
//
void fft(const SDoublePlane &input, SDoublePlane &fft_real, SDoublePlane &fft_imag)
{
  fft_real = input;
  fft_imag = SDoublePlane(input.rows(), input.cols());

  FFT_2D(1, fft_real, fft_imag);
}

// Inverse FFT transform: take real and imaginary parts of fourier transform, and return
//  real-valued image.
//
void ifft(const SDoublePlane &input_real, const SDoublePlane &input_imag, SDoublePlane &output_real)
{
  output_real = input_real;
  SDoublePlane output_imag = input_imag;

  FFT_2D(0, output_real, output_imag);
}

// Write this in Part 1.1
SDoublePlane fft_magnitude(const SDoublePlane &fft_real, const SDoublePlane &fft_imag){
  double max = 0.0;
  double min = 99999.0;
  SDoublePlane output_log_magnitude = SDoublePlane(fft_real.rows(), fft_real.cols());
  
  for(int i = 0; i < fft_real.rows(); i++){
    for(int j = 0; j < fft_real.cols(); j++){
      output_log_magnitude[i][j] = log10(sqrt(pow(fft_real[i][j], 2.0) + pow(fft_imag[i][j], 2.0)));
      if(output_log_magnitude[i][j] > max){
		    max = output_log_magnitude[i][j];
      }
      if(output_log_magnitude[i][j] < min){
        min = output_log_magnitude[i][j];
      }
    }
  }
  double c = 255 / (max - min);
  for(int i = 0; i < fft_real.rows(); i++){
    for(int j = 0; j < fft_real.cols(); j++){
      output_log_magnitude[i][j] = c * (output_log_magnitude[i][j] - min);
    }
  }
  return output_log_magnitude;
}



// Write this in Part 1.2
SDoublePlane remove_interference(const SDoublePlane &input){
  SDoublePlane fft_real, fft_imag, ifft_real;
  fft_real = input;
  fft_imag = SDoublePlane(input.rows(), input.cols());
  fft(input, fft_real, fft_imag);

  // manually removing noise
  for(int i = 0; i < 165; i++){
    for(int j = 0; j < fft_real.cols(); j++){
      fft_real[i][j] = 0;
      fft_imag[i][j] = 0;
    }
  }
  for(int i = 350; i < fft_real.rows(); i++){
    for(int j = 0; j < fft_real.cols(); j++){
      fft_real[i][j] = 0;
      fft_imag[i][j] = 0;
    }
  }
  ifft(fft_real, fft_imag, ifft_real);
  return ifft_real;
}



// Write this in Part 1.3 -- add watermark N to image
SDoublePlane mark_image(const SDoublePlane &input, int N){

  SDoublePlane real_part = input;
  SDoublePlane imaginary_part = SDoublePlane(input.rows(), input.cols());
  SDoublePlane new_real_part = input;

  // Perform Discrte Fourier Transform
  fft(input, real_part, imaginary_part);
  
  // Parameters
  int length = 200;
  int radius = 175;
  int alpha = 5;
  int row_vector[length];

  // Generate pseudorandom numbers using seed
  srand(N);
  for(int i = 0; i < length; i++){
    row_vector[i] = rand() % 2;
  }

  // Insert watermark bits into bins
  int rows = input.rows();
  int cols = input.cols();
  for(int i = 1; i <= length; i++){
    int x = (rows/2 + 1) + floor(radius * cos(i * M_PI / length));
    int y = (cols/2 + 1) + floor(radius * sin(i * M_PI / length));
    int x1 = (rows/2 + 1) + floor(radius * cos((i * M_PI / length) + M_PI));
    int y1 = (cols/2 + 1) + floor(radius * sin((i * M_PI / length) + M_PI));

    real_part[x][y] = real_part[x][y] + (alpha * fabs(real_part[x][y]) * row_vector[i-1]);
    real_part[x1][y1] = real_part[x1][y1] + (alpha * fabs(real_part[x1][y1]) * row_vector[i-1]);
  }
  //Perform Inverse Discrete Fourier Transform
  ifft(real_part, imaginary_part, new_real_part);
  return new_real_part;
}



double get_mean(double arr[], int length){
  double sum = 0.0;
  for(int i = 0; i < length; i++){
    sum = sum + arr[i];
  }
  sum = sum / length;
  return sum; 
}



// Write this in Part 1.3 -- check if watermark N is in image
void check_image(const SDoublePlane &input, int N){
  
  SDoublePlane real_part = input;
  SDoublePlane imaginary_part = SDoublePlane(input.rows(), input.cols());
  
  // Perform Discrte Fourier Transform
  fft(input, real_part, imaginary_part);
  
  // Parameters
  int length = 200;
  int radius = 175;
  int alpha = 5;
  double row_vector[length];
  double retrieved_row_vector[length];

  // Generate pseudorandom numbers using seed
  srand(N);
  for(int i = 0; i < length; i++){
    row_vector[i] = rand() % 2;
  }

  int rows = input.rows();
  int cols = input.cols();
  for(int i = 1; i <= length; i++){
    int x = (rows/2 + 1) + floor(radius * cos(i * M_PI / length));
    int y = (cols/2 + 1) + floor(radius * sin(i * M_PI / length));
    retrieved_row_vector[i] = real_part[x][y];
  }
  double mean_row_vector = get_mean(row_vector, length);
  double mean_retrieved_row_vector = get_mean(retrieved_row_vector, length);

  double pearson_cor_coefficient = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;
  for(int i = 0; i < length; i++){
    sum1 = sum1 + ((row_vector[i] - mean_row_vector) * (retrieved_row_vector[i] - mean_retrieved_row_vector));
    sum2 = sum2 + pow((row_vector[i] - mean_row_vector), 2.0);
    sum3 = sum3 + pow((retrieved_row_vector[i] - mean_retrieved_row_vector), 2.0);
  }
  pearson_cor_coefficient = sum1 / (sqrt(sum2) * sqrt(sum3));
  if(pearson_cor_coefficient >= -0.1){
    cout << "Test positive." << endl;
  } else{
    cout << "Test negative" << endl;
  }
}

int main(int argc, char **argv)
{
  try {

    if(argc < 4)
    {
      cout << "Insufficent number of arguments; correct usage:" << endl;
      cout << "    p2 problemID inputfile outputfile" << endl;
      return -1;
    }
    
    string part = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    cout << "In: " << inputFile <<"  Out: " << outputFile << endl;
    
    SDoublePlane input_image = SImageIO::read_png_file(inputFile.c_str());
    SDoublePlane real_part, imaginary_part;
    
    if(part == "1.1")
    {
      fft(input_image, real_part, imaginary_part);
      SDoublePlane output_image = fft_magnitude(real_part, imaginary_part);
      SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image);
    }
    else if(part == "1.2")
    {
      SDoublePlane output_image = remove_interference(input_image);
      SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image);      
    }
    else if(part == "1.3")
    {
      if(argc < 6)
      {
        cout << "Need 6 parameters for watermark part:" << endl;
        cout << "    p2 1.3 inputfile outputfile operation N" << endl;
        return -1;
      }
      string op(argv[4]);
      if(op == "add")
      {
        // add watermark
        int n = atoi(argv[5]);
        SDoublePlane output_image = mark_image(input_image, n);
        SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image); 
      }
      else if(op == "check")
      {
        //check watermark
        int n = atoi(argv[5]);
        check_image(input_image, n);
      }
      else
        throw string("Bad operation!");

      int N = atoi(argv[5]);
    }
    else
      throw string("Bad part!");
  } 
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}

/* REPORT */
/* 
Value of radius i skept at 175 (middle frequency range) so 
that we do not affect low frequency much and cause visible damage to image.

Similaraly value of alpha is also kept low at 5 so as to not cause any high intensity changes in image 
resulting in blurry noise effect and ringing.

Length of vector is 200 so that embedded bits do not get lost on post image processing and can be retrieved easily.

Value of threshold for watermark detection is kept at -0.1 so that even after editing we can easily estimate prescence 
of watermark in image.

*/
