#include "node.h"
#include "parameter.h"
#include "Ucache.h"

Node::Node(int n_atiles, const ATile & atile1, int buffer_size, double cycle)
		  :n_atiles(n_atiles), atile(atile1), size(buffer_size),cycle(cycle)
{
	wire_length = n_atiles* sqrt(atile.area(0.032,16))/2;
	//cout << "wire_length: " << wire_length << endl;
}


double Node::throughput(int n_bits_per_number)
{
		return n_atiles*atile.throughput(n_bits_per_number); 
}

double Node::area_buffer(double tech)
{
  if(size <= 1024)
	return size*2*tech*tech;
  g_ip->nbanks = 2;
  g_ip->line_sz = atile.array.H * 2;
  g_ip->cache_sz = size;
  //g_ip->line_sz = num_wires/8;
  
 //// cout << "area_buffer::num_wires " <<num_wires << endl;
  if( num_wires > (8*g_ip->line_sz))
  {
	g_ip->nbanks *= num_wires/(8*g_ip->line_sz);
	////cout << "/" << g_ip->nbanks ;
	g_ip->out_w = g_ip->line_sz;
  }
  else	
  {
	g_ip->out_w = num_wires;
  }  
  
  g_ip->data_arr_ram_cell_tech_type =0;
  
  g_ip->is_cache=false;
  g_ip->data_arr_peri_global_tech_type = 0;
  
  uca_org_t fin_res;
  
  solve(&fin_res);
  
  int n_bits_per_number =16;
  power_central_io_dyn = 1000*(2*fin_res.data_array2->power.readOp.dynamic * 1e9 * atile.n_adcs * (atile.array.H*n_bits_per_number/512.0)) / atile.array.T;
  //power_central_io_dyn = 1000*(2*fin_res.data_array2->power.readOp.dynamic * 1e9 * atile.n_adcs * (atile.array.H*n_bits_per_number/(atile.array.H*n_bits_per_number))) / atile.array.T;

  power_central_io_leak = fin_res.data_array2->power.readOp.leakage*1e3;
  
  return fin_res.data_array2->area;
}
	
double Node::area_router(double tech)
{
	return 151597/4.0;
	//return 63000;
}
	
double Node::area_bus(double tech, int n_bits_per_number)
{
	double pitch = 0.1688 + 0.151; //0.112+0.95 (metal 3);
	
	int length = n_atiles* sqrt(atile.area(tech,n_bits_per_number))/2;
	
	
	///cout << "length: "<< length << endl;
	
	//double time_per_tile = atile.array.T * ((double)n_bits_per_number)/(2*n_atiles); // in ns
	
	double time_per_tile = atile.array.T * 0.64 * ((double)n_bits_per_number)/(2*n_atiles*atile.array.res_dac); // in ns
	
	
	int data_in_size_per_tile = atile.n_adcs * atile.array.H * n_bits_per_number;
	
	int n_in_wires = cycle * data_in_size_per_tile/time_per_tile;
	
	
	int data_out_size_per_tile = atile.n_adcs * (atile.array.W/atile.n_cols_per_output) * n_bits_per_number/8;
	
	int n_out_wires = cycle * data_out_size_per_tile/time_per_tile;
	
	int n_wire = n_in_wires>n_out_wires?n_in_wires:n_out_wires;
	
	///cout << "n_wire: " << n_wire << endl;
	//if(n_wire<1024) n_wire=1024;
	
	if((n_wire*pitch*n_atiles/2)>length)
		length = (n_wire*pitch*n_atiles/2);
	
	num_wires = n_wire;
	///cout << "n_wires " <<n_wire << "  ";
	
	return length* pitch * n_wire;
	
}
	
double Node::area_sigmoid(double tech, int n_bits_per_number)
{
	int n_write_back = (atile.array.W/atile.n_cols_per_output) * n_bits_per_number/8;
	
	double transistor_area = 2*tech * tech; 
	double area_table = 6 * 12 * n_bits_per_number* transistor_area ;
	double area_arith = (14*n_bits_per_number + 14*n_bits_per_number*n_bits_per_number)*transistor_area;
	
	///cout << n_write_back<< "-" << transistor_area<< "-"<< area_table<< "-"<< area_arith<< "\n";
	return 2*n_write_back*(area_table+area_arith);
	
}
	
double Node::area(double tech, int n_bits_per_number)
{
	///cout << "(buf: " << area_buffer(tech) << ",til: " <<n_atiles* atile.area(tech,n_bits_per_number) 
	///	 << ",bus: " << area_bus(tech,n_bits_per_number) << ",sgm: " <<area_sigmoid(tech,n_bits_per_number) << " " << area_router(tech) << ")\n";
	double area_tile =  n_atiles* atile.area(tech,n_bits_per_number);
	double area_bs = area_bus(tech,n_bits_per_number);
	double area_smg = area_sigmoid(tech,n_bits_per_number);
	double area_rt = area_router(tech);
	double area_bf = area_buffer(tech);
	//double result =  n_atiles* atile.area(tech,n_bits_per_number) + area_bus(tech,n_bits_per_number) + area_sigmoid(tech,n_bits_per_number)+ area_router(tech);
	//result += area_buffer(tech) ;
	double result = area_tile + area_bs + area_smg + area_rt + area_bf ;
	//cout << " ^^^^ "<< area_tile << "/" << area_bs << "/" << area_smg << "/" << area_rt << "/"<< area_bf << " ";
	return result;
}

double Node::power(int n_bits_per_number)
{
	double total_power = n_atiles*atile.power(n_bits_per_number);
	
	double power_bus = num_wires*0.026105812*wire_length/1000.0;
	
	//cout << "--- " << power_bus << endl;
	
	total_power += power_central_io_dyn;
	total_power += power_central_io_leak;
	total_power += power_bus;
	total_power += 0.0427485 *1000;//*n_atiles/12;
	//cout << power_central_io_dyn << " " << power_central_io_leak << " " << power_bus << " ";
	//cout << endl;
	///cout << (power_central_io_dyn+power_central_io_leak) << " " << n_atiles*atile.power(n_bits_per_number) << " " << power_bus << " 0 " << 0.0427485 *1000 << endl;
	
	return total_power;
}
	
