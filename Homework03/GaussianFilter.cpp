#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif
//#include "directives.h"
#include "GaussianFilter.h"

GaussianFilter::GaussianFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_MAP_TO_REG_BANK(rgb);
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	for(int i = 0; i < 9; i++){
		i_rgb[i].clk_rst(i_clk, i_rst);
	}
	o_result.clk_rst(i_clk, i_rst);
#endif
}

GaussianFilter::~GaussianFilter() {}

// Gaussian mask
const int mask[MASK_X][MASK_Y] = {{1,2,1},{2,4,2},{1,2,1}};

void GaussianFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		for(int i = 0; i < 9; i++){
			i_rgb[i].reset();
		}
		o_result.reset();
#endif
		wait();//Why we wait here?
	}
	while (true) {
		//PIPELINE;
		HLS_PIPELINE_LOOP(SOFT_STALL, 2, "Main" );
		{
			HLS_CONSTRAIN_LATENCY(0, 1, "compute");
			r_val = 0;
			g_val = 0;
			b_val = 0;
		}
		
		//READ BLOCK
		for (unsigned int i = 0; i<9; ++i) {
			//CONV_LOOP;
			HLS_UNROLL_LOOP( ON, "conv_loop" );
			HLS_CONSTRAIN_LATENCY(0, 2, "lat01");
			#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb[i] = i_rgb[i].get();
					wait();
				}
			#else
				rgb[i] = i_rgb[i].read();
			#endif
		}
		//COMPUTE PART I
		{
			HLS_CONSTRAIN_LATENCY(0, 4, "compute");
			r_val = (((rgb[0].range(7,0) >> 4) + (rgb[1].range(7,0) >> 3))  + 
			         ((rgb[2].range(7,0) >> 4) + (rgb[3].range(7,0) >> 3))) +
					(((rgb[4].range(7,0) >> 2) + (rgb[5].range(7,0) >> 3))  +
					 ((rgb[6].range(7,0) >> 4) + (rgb[7].range(7,0) >> 3))) +
					  (rgb[8].range(7,0) >> 4);
			g_val = (((rgb[0].range(15,8) >> 4) + (rgb[1].range(15,8) >> 3))  + 
			         ((rgb[2].range(15,8) >> 4) + (rgb[3].range(15,8) >> 3))) +
					(((rgb[4].range(15,8) >> 2) + (rgb[5].range(15,8) >> 3))  +
					 ((rgb[6].range(15,8) >> 4) + (rgb[7].range(15,8) >> 3))) +
					  (rgb[8].range(15,8) >> 4);
			b_val = (((rgb[0].range(23,16) >> 4) + (rgb[1].range(23,16) >> 3))  + 
			         ((rgb[2].range(23,16) >> 4) + (rgb[3].range(23,16) >> 3))) +
					(((rgb[4].range(23,16) >> 2) + (rgb[5].range(23,16) >> 3))  +
					 ((rgb[6].range(23,16) >> 4) + (rgb[7].range(23,16) >> 3))) +
					  (rgb[8].range(23,16) >> 4);
		}
		
		{
			HLS_CONSTRAIN_LATENCY(0, 1, "lat_02");
			o_rgb.range(7,0)   = r_val;
			o_rgb.range(15,8)  = g_val;
			o_rgb.range(23,16) = b_val;
		}
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(o_rgb);
			wait();
		}
#else
		o_result.write(o_rgb);
#endif
	}
}
