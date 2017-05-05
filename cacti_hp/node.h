#ifndef __NODE_H__
#define __NODE_H__
#include "tile.h"

/**
class DTile
{
	public:
	int n_alus;
	int n_multipliers;
	int n_bits_per_number;

	DTile( int n_ouput_per_T, int n_bits_per_number);
	double throughput();
	double area();
	double power();
};
**/

class Node
{
	public:
	
	int n_atiles;
	ATile atile;
	
	int size;
	double cycle;
	
	int num_wires;
	double wire_length;
	
	double power_central_io_dyn;
	double power_central_io_leak;
	
	Node(int n_atiles, const ATile &atile, int buffer_size, double cycle);
	double throughput(int n_bits_per_number);
	double area_buffer(double tech);
	double area_router(double tech);
	double area_bus(double tech, int n_bits_per_number);
	double area_sigmoid(double tech, int n_bits_per_number);
	
	double area(double tech,int n_bits_per_number);
	double power(int n_bits_per_number);
	
	
	

};

#endif
