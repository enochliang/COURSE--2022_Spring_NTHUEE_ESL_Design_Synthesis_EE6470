#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif
//#include "directives.h"
#include "MatrixMult.h"

Matrix_Mult::Matrix_Mult( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_MAP_TO_REG_BANK(W);
	HLS_MAP_TO_REG_BANK(IA);
	HLS_MAP_TO_REG_BANK(OA);
#endif
	SC_THREAD( do_mult );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
	i_data.clk_rst(i_clk, i_rst);
	o_result.clk_rst(i_clk, i_rst);
#endif
}

Matrix_Mult::~Matrix_Mult() {}


void Matrix_Mult::do_mult() {
	{
		#ifndef NATIVE_SYSTEMC
			HLS_DEFINE_PROTOCOL("main_reset");
			i_data.reset();
			o_result.reset();
		#endif
		wait();//Why we wait here?
	}
	while (true) {
		

		#ifndef NATIVE_SYSTEMC
			//HLS_CONSTRAIN_LATENCY(0, 2, "input_weight");
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				HLS_CONSTRAIN_LATENCY("input_weight");
				HLS_DEFINE_PROTOCOL("input");
				data_wire = i_data.get();
				mode = data_wire[32];
				for( unsigned int k = 0 ; k < 4 ; k++ ){
					HLS_UNROLL_LOOP( ON, "get_weight" );
					W[k][i] = data_wire.range((8*k)+7,8*k);
				}
			}
			wait();
			
		#else
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				data_wire = i_data.read();
				for( unsigned int k = 0 ; k < 4 ; k++ ){
					W[k][i] = data_wire.range((8*k)+7,8*k);
				}
			}
		#endif

		{
		HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
		//input_activation
		#ifndef NATIVE_SYSTEMC
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				HLS_CONSTRAIN_LATENCY("input_ia");
				HLS_DEFINE_PROTOCOL("input");
				data_wire = i_data.get();
				for( unsigned int k = 0 ; k < 4 ; k++ ){
					HLS_UNROLL_LOOP( ON, "get_weight" );
					IA[k][i] = data_wire.range((8*k)+7,8*k);
				}
			}
			wait();
		#else
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				data_wire = i_data.read();
				for( unsigned int k = 0 ; k < 4 ; k++ ){
					IA[k][i] = data_wire.range((8*k)+7,8*k);
				}
			}
		#endif

		//COMPUTE BLOCK
		if(mode){
			for (unsigned int i = 0; i < 4 ; ++i ) {
				for(unsigned int j = 0; j < 4 ; ++j ){
					//HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
					{
					HLS_CONSTRAIN_LATENCY("lat_r");
					OA[i][j] = 0;
					}
					for(unsigned int n = 0; n < 4 ; ++n ){
						HLS_CONSTRAIN_LATENCY("lat_c");
						HLS_UNROLL_LOOP( ON, "compute" );
						Tmp = IA[n][i]*W[n][j];
						OA[i][j] += Tmp;
					}
				}
			}
		}else{
			for (unsigned int i = 0; i < 4 ; ++i ) {
				for(unsigned int j = 0; j < 4 ; ++j ){
					HLS_CONSTRAIN_LATENCY("lat_add");
					OA[i][j] = IA[i][j]+W[i][j];
				}
			}
		}

		
		}

		//OUTPUT BLOCK
		#ifndef NATIVE_SYSTEMC
			for (unsigned int i = 0; i < 4 ; ++i ) {
				HLS_CONSTRAIN_LATENCY("lat_o");
				for (unsigned int j = 0; j < 4 ; ++j ) {
					HLS_UNROLL_LOOP( ON, "owire" );
					o_data_wire.range(16*j+15,16*j)=OA[i][j];
				}
				{
					HLS_DEFINE_PROTOCOL("output");
					o_result.put(o_data_wire);
				}
			}
			wait();
		#else
			for (unsigned int i = 0; i < 4 ; ++i ) {
				for (unsigned int j = 0; j < 4 ; ++j ) {
					o_data_wire.range(16*j+15,16*j)=OA[i][j];
				}
				o_result.write(o_data_wire);
			}
		#endif

	}
}
