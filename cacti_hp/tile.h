#ifndef __TILE_H__
#define __TILE_H__

#include "htree2.h"

// area in number of transistor
extern const double DAC_area[16]; // area for different resolution
extern const double s_and_h_area;



class Array
{
	public:
	int H;
	int W;
	int cols_per_output;	
	int res_adc;
	int res_dac;
	double T; // in nano-sec	

	Array(int height, int width, int cols_per_output
	     , int res_adc, int res_dac, double critical_path);

	Array(const Array& array);
	
	double throughput(int n_bits_per_number); // in GOPs/s
	double area(double tech); // in um^2
	double power();

};



class ATile
{
	public:
	
	int n_arrays;
	Array array;
	
	Htree2 *htree_in_addr;
	Htree2 *htree_in_data;
    Htree2 *htree_out_data;
	
	
	int n_adcs;
	int n_cols_per_output;
	
	double cycle;
	
	ATile(int num_arrays, int num_adcs, int num_cols_per_outputs, const Array & array, double cycle);
	ATile(const ATile & atile);
	double throughput( int n_bits_per_number);
	double array_area(double tech);
	double adc_area (); 
	double buffer_area(double tech, int type, int size);
	double area(double tech, int n_bits_per_number);
	double adc_power();
	double power(int n_bits_per_number);
};

#endif
