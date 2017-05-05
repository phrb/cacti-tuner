#include "tile.h"
#include "parameter.h"
#include "Ucache.h"
#include <cassert>

extern const double DAC_area[16] 
//={80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80};
={500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500};
extern const double s_and_h_area = 40;


int log2_int(int value)
{
	assert(value>0);
	int counter = -1;
	while(value>0)
	{
		value/=2;
		counter++;
	}
	return counter;
}


Array::Array(int height, int width, int cols_per_output
             , int res_adc, int res_dac, double critical_path)
	    :H(height),W(width),cols_per_output(cols_per_output)
	    ,res_adc(res_adc), res_dac(res_dac), T(critical_path)
{}

Array::Array(const Array & array)
			:H(array.H),W(array.W),cols_per_output(array.cols_per_output)
			,res_adc(array.res_adc), res_dac(array.res_dac), T(array.T)
{}

double Array::throughput(int n_bits_per_number)
{

	return 2*H*(W/cols_per_output)/((n_bits_per_number/res_dac)*T);
}

double Array::area(double tech) // in um^2
{
	double transistor_area = 2*tech * tech;
	double array_area = H*W*transistor_area;
	double dac_area = H*DAC_area[0]*transistor_area;
	double ff_area = H* 12 * transistor_area;
	double s_h_area = W * s_and_h_area * transistor_area;
	double mux_area = (W-1)*2*transistor_area;

	return ( array_area + dac_area + ff_area + s_h_area + mux_area);
}

double Array::power() // in mWatt
{
	return 0.5;
}


ATile::ATile(int num_arrays, int num_adcs, int num_cols_per_output, const Array & array, double cycle)
			:n_arrays(num_arrays), n_adcs(num_adcs), n_cols_per_output(num_cols_per_output), array(array),cycle(cycle)
{}

ATile::ATile(const ATile & atile)	
			:n_arrays(atile.n_arrays), n_adcs(atile.n_adcs), n_cols_per_output(atile.n_cols_per_output), array(atile.array), cycle(atile.cycle)
{}			
	
double ATile::throughput( int n_bits_per_number)
{
	return n_adcs* array.throughput(n_bits_per_number);
}

double ATile::array_area(double tech)
{
	if(n_arrays<=2)
		return n_arrays*array.area(tech);
		
	int total_address_bit = 0;
	int total_data_in_bit=  log2_int(n_adcs) + n_adcs*array.H /128;

	int	total_data_out_bit= array.res_adc*n_adcs;
	
	int num_mats_ver_dir;
	int num_mats_hor_dir;
	

	num_mats_ver_dir = (log2_int(n_arrays)%2==1)? 0x1<< (log2_int(n_arrays*2)/2) : 0x1<< (log2_int(n_arrays)/2);
	num_mats_hor_dir = n_arrays/num_mats_ver_dir;
	
	assert( n_arrays <=(num_mats_ver_dir*num_mats_hor_dir));
	
	htree_in_addr = new Htree2 (g_ip->wt, sqrt(array.area(tech)+adc_area()), sqrt(array.area(tech)+adc_area()),
	total_address_bit,total_data_in_bit , total_data_out_bit, 2*num_mats_ver_dir, 2*num_mats_hor_dir,Add_htree);
	
	htree_in_data = new Htree2 (g_ip->wt, sqrt(array.area(tech)+adc_area()), sqrt(array.area(tech)+adc_area()),
	total_address_bit,total_data_in_bit , total_data_out_bit, 2*num_mats_ver_dir, 2*num_mats_hor_dir,Data_in_htree);
	
	htree_out_data = new Htree2 (g_ip->wt, sqrt(array.area(tech)+adc_area()), sqrt(array.area(tech)+adc_area()),
	total_address_bit,total_data_in_bit , total_data_out_bit, 2*num_mats_ver_dir, 2*num_mats_hor_dir,Data_out_htree);
	
	double result = htree_in_data->area.w * htree_in_data->area.h;

	free(htree_in_data);
	return result;
}
	
double ATile::adc_area() // um^2
{

	double area = 1500;
	
	return area;
}	

double ATile::adc_power()
{
	double power = 3.1 * ((array.res_adc + 3)/11.0);
	power = power + 0.0356*(array.W-128);
	return power;
}

double ATile::buffer_area(double tech, int type, int size)
{
  if(size <= 1024)
	return size*24*tech*tech;
  g_ip->nbanks =2;
  g_ip->line_sz = array.H*2;
  g_ip->out_w = array.H;
  g_ip->cache_sz = size;
  g_ip->data_arr_ram_cell_tech_type =type;
  
   g_ip->is_cache=false;
  g_ip->data_arr_peri_global_tech_type = 0;
  
  uca_org_t fin_res;
  
  solve(&fin_res);
  
  return fin_res.data_array2->area;
}
	
double ATile::area(double tech,int n_bits_per_number)
{
	double transistor_area = 2*tech * tech; 
	double input_unit_area = buffer_area(tech, 0, n_bits_per_number * array.H * n_adcs/8.0) ;
	double output_unit_area = buffer_area(tech, 0, (array.W/array.cols_per_output) * n_adcs * n_bits_per_number /8.0);
	double mvm_area = array_area(tech);
	double arith_area = 2*n_bits_per_number * (12+14) * transistor_area;
	return (input_unit_area + output_unit_area + mvm_area +arith_area);
	
}

double ATile::power(int n_bits_per_number)
{
	int size = n_bits_per_number * array.H * n_adcs/8;
	
	g_ip->line_sz = array.H*2;
	
	if(size <= (array.H*2)) size=(array.H*2)*2;
	
	g_ip->nbanks =1;
	g_ip->cache_sz = size;
	g_ip->out_w = array.H;
	g_ip->data_arr_ram_cell_tech_type =0;
	g_ip->is_cache=false;
    g_ip->data_arr_peri_global_tech_type = 0;
	uca_org_t fin_res;
	solve(&fin_res);
	double power_input_unit = 1000*(fin_res.data_array2->power.readOp.dynamic * 1e9 * 1 * n_adcs *2) / array.T;
    
	size = (array.W/array.cols_per_output) * n_adcs * n_bits_per_number /8;
	
	g_ip->line_sz = array.H*2;
	
	if(size < (2*array.H*2)) size=(array.H*2)*2;	
	
	g_ip->nbanks =1;
	g_ip->cache_sz = size;
	g_ip->out_w = array.H *2;
	g_ip->data_arr_ram_cell_tech_type =0;
	g_ip->is_cache=false;
    g_ip->data_arr_peri_global_tech_type = 0;
	uca_org_t fin_res1;
	solve(&fin_res1);
	double power_output_unit = 1000*(fin_res1.data_array2->power.readOp.dynamic * 1e9 * 1 * n_adcs *2) /array.T;
	
	return ( n_adcs * (adc_power() + array.power()) + power_input_unit + power_output_unit ); 
}
