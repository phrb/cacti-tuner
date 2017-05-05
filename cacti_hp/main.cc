#include "io.h"
#include <iostream>

#include "parameter.h"
#include "tile.h"
#include "Ucache.h"
#include "node.h"

using namespace std;


int main(int argc,char *argv[])
{

  uca_org_t result;

  assert(argc==3);
  
  init_cacti(argv[2]);
  
  assert(g_ip!=NULL);
  
  
  g_ip->nbanks =2;
  g_ip->line_sz = 256;
  g_ip->out_w = 128;
  g_ip->cache_sz = 1024*4*1;
  g_ip->data_arr_ram_cell_tech_type =3;
  g_ip->is_cache=false;
  g_ip->data_arr_peri_global_tech_type = 2;
  uca_org_t fin_res;
  
  solve(&fin_res);
  
  cout << "  Access time (ns): " << fin_res.access_time*1e9 << endl;
  cout << "  power : ( " << fin_res.power.readOp.dynamic << ","  << fin_res.power.readOp.leakage << ")\n";
  cout << "  Area: " << fin_res.data_array2->area * 1e-6 << endl;
  

  
  
  for(int i=6;i<=8;i++)
  {
	  int j = 16/(9-i);
	  if(j==5) j=6;
	  cout << j <<": \n";
	  int size = 0x1<<i;	

	  Array array1(size, size, j, 8, 1, 100);
	  //Array array2(size, 128, j, 8, 1, 100);
	  
	  for(int k=32;k<=32;k*=2)		//k = #adc, or (logical_row * logical_col)
	  {
		  for(int r=1;r<=1;r*=4)
		  {
		  ATile tile1(r*k, k, j, array1, 1);
		  //ATile tile2(r*k, k, j, array2, 1);
		  cout << "H"<< size << "-A" << k << "-R"<<r<< " ";
		  
		  
		  
		  //cout << 1e6*tile.throughput(16)/tile.area(0.032,16) << " ";
			  for(int n=4;n<=4;n*=2)
			  {
				Node node(n,tile1,64*1024,1); 
				double area = node.area(0.032,16);
				double power = node.power(16);
				
				cout << 1000000*node.throughput(16)/area << " ";
				
				//cout << (power/1000.0) << " ";
				//cout << 1000*power/area << " ";
				
				//cout << node.area(0.032,16)/1000000.0 << " ";	
				
				//cout << power/1000.0 << " ";
				//cout << 1000*node.throughput(16)/power << " ";
				//cout << (1000000.0/area)*( ((256*256/1024)*2*node.n_atiles*node.atile.n_arrays/8)/1024.0 ) << " "; 
				//cout << (1000/node.area(0.032,16)) *(1000000*node.throughput(16)/node.area(0.032,16)) << " ";
				//cout << node.power(16) << " ";
				//cout << node.area(0.032,16) << " ";
				//cout << 1000*node.throughput(16) << " "
			  }
			  cout << endl;
		  }
		  cout << endl;
	  }
	  cout << endl;
  }
  cout << endl << endl;
  return 0;
}

