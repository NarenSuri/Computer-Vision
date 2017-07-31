#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <SImage.h>
#include <SImageIO.h>
#include <fft.h>

using namespace std;


// This code requires that input be a *square* image, and that each dimension
// //  is a power of 2; i.e. that input.width() == input.height() == 2^k, where k
// //  is an integer. You'll need to pad out your image (with 0's) first if it's
// //  not a square image to begin with. (Padding with 0's has no effect on the FT!)
// //
// // Forward FFT transform: take input image, and return real and imaginary parts.
// //
void fft(const SDoublePlane &input, SDoublePlane &fft_real, SDoublePlane &fft_imag)
{
	fft_real = input;
	fft_imag = SDoublePlane(input.rows(), input.cols());

	FFT_2D(1, fft_real, fft_imag);
 }

void ifft(const SDoublePlane &input_real, const SDoublePlane &input_imag, SDoublePlane &output_real)
{
  output_real = input_real;
  SDoublePlane output_imag = input_imag;

  FFT_2D(0, output_real, output_imag);
}

