#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <float.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <tgmath.h>
#include <assert.h>

using namespace std;

#define uglyrows 170
#define rows 128
#define columns 128
#define dac 1
#define precision 16
#define sharedkernel 1
#define carry 0
#define heterocols 0
#define inputlsbtomsb 1


int pow2_int(int value)
{
	assert(value>0);
	int result = 1;
	for(int i=0; i<value; i++)
		result *= 2;
	return result;
}

int log2_int(int value)
{
        assert(value>0);
        int original = value;
        int counter = -1;
        while(value>0)
        {
                value/=2;
                counter++;
        }
        if(pow2_int(counter)<original)
			return counter+1;
		else
			return counter;
}

int main(int argc, const char *argv[])
{
	int prevxI = atoi(argv[1]);
	int prevxF = atoi(argv[2]);
	int prevwI = atoi(argv[3]);
	int prevwF = atoi(argv[4]);
	int nextxI = atoi(argv[5]);
	int nextxF = atoi(argv[6]);
	int cols_per_weight = atoi(argv[7]);
	int* bpc = new int[cols_per_weight];
	
	cout << "Enter the bits per column :\n";
	for(int k=0; k < cols_per_weight; k++)
		cin >> bpc[k];

	int itr = 0, itr_flag = 0, col_flag = 0;
	if ((prevxI + prevxF)%dac != 0)	itr_flag = 1;

	itr = (prevxI + prevxF)/dac + itr_flag;
	

	int i,j;
	
/*
	int col_res = log2_int(rows) + bpc + dac;
	if(dac == 1) col_res -= 1;
	if(bpc == 1) col_res -= 1;
	
	int flag_col_res = log2_int(rows) + ((prevwI + prevwF)%bpc) + dac;
	if(dac == 1) flag_col_res -= 1;
	if((prevwI + prevwF)%bpc == 1) flag_col_res -= 1; 

	int msb_col_res = log2_int(rows) + bpc + ((prevxI + prevxF)%dac);
	if((prevxI + prevxF)%dac == 1) msb_col_res -= 1;
	if(bpc == 1) msb_col_res -= 1;

	int flag_msb_col_res = log2_int(rows) +  ((prevwI + prevwF)%bpc) + ((prevxI + prevxF)%dac);
	if((prevxI + prevxF)%dac == 1) flag_msb_col_res -= 1;
	if((prevwI + prevwF)%bpc == 1) flag_msb_col_res -= 1;
*/
	int col_res[itr][cols_per_weight];
	

	int bits_per_itr = 0, bits_to_sample = 0, msb_bits_not_needed = 0;

	int outxI = prevxI + prevwI + log2_int(rows);
	int outxF = prevxF + prevwF;

	for(i=0; i < itr; i++)
	{
		for(j=0; j < cols_per_weight; j++)
		{
			if(i== (inputlsbtomsb?itr-1:0) && itr_flag == 1)
			{
				col_res[i][j] = log2_int(rows * (pow2_int(bpc[j]) - 1) * (pow2_int((prevxI+prevxF)%dac) - 1));
			}
			else
			{
				col_res[i][j] = log2_int(rows * (pow2_int(bpc[j]) - 1) * (pow2_int(dac) - 1));
			}
		}
	}
	
	for(i = 0; i < itr; i++)
	{
		bits_per_itr = log2_int(rows* (pow2_int(prevwI + prevwF)-1) * (pow2_int(dac)-1));
		bits_to_sample = bits_per_itr + i*dac - (outxF - nextxF);
		
			
		if(i == (inputlsbtomsb?itr-1:0) && itr_flag == 1) 
		{
			bits_per_itr = log2_int(rows* (pow2_int(prevwI + prevwF)-1) * (pow2_int((prevxI+prevxF)%dac)-1));
			bits_to_sample = bits_per_itr + i*((prevxI+prevxF)%dac) - (outxF - nextxF);
		}
		
		
		if(bits_to_sample > (nextxI + nextxF))
		{
			msb_bits_not_needed = bits_to_sample - (nextxI + nextxF);
			if(bits_to_sample >= bits_per_itr)
				bits_to_sample = bits_per_itr - msb_bits_not_needed;
			else
				bits_to_sample -= msb_bits_not_needed;
		}

		
		for(j = 0; j < cols_per_weight; j++)
		{
				
				if(!msb_bits_not_needed)
				{
					if(bits_to_sample > col_res[i][j])
					{
						bits_to_sample -= bpc[j];
					}
					else
					{
						col_res[i][j] = bits_to_sample>0?bits_to_sample:0;
						bits_to_sample -= bpc[j];
					}
				}
				else
				{
					col_res[i][j] = (col_res[i][j] - msb_bits_not_needed)>0?(col_res[i][j] - msb_bits_not_needed):0;
					msb_bits_not_needed -= bpc[j];

					if(msb_bits_not_needed < 0) 
					{
						bits_to_sample += msb_bits_not_needed; /*check*/
						msb_bits_not_needed = 0;
					}
				}
		}
	}
	
	for(i = 0; i < itr; i++)
	{
		for(j = 0; j < cols_per_weight; j++)
		{
			cout << col_res[i][j] << "\t";
		}
		cout << "\n";
	}
		
}
