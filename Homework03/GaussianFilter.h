#ifndef GAUSSIAN_FILTER_H_
#define GAUSSIAN_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class GaussianFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> >::in i_rgb[9];
	cynw_p2p< sc_dt::sc_uint<24> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_uint<24> > i_rgb[9];
	sc_fifo_out< sc_dt::sc_uint<24> > o_result;
#endif

	SC_HAS_PROCESS( GaussianFilter );
	GaussianFilter( sc_module_name n );
	~GaussianFilter();
private:
	void do_filter();
    unsigned int r_val;
	unsigned int g_val;
	unsigned int b_val;

	unsigned int r_L1[5];
	unsigned int g_L1[5];
	unsigned int b_L1[5];

	unsigned int r_L2[3];
	unsigned int g_L2[3];
	unsigned int b_L2[3];

	unsigned int r_L3[2];
	unsigned int g_L3[2];
	unsigned int b_L3[2];
	sc_dt::sc_uint<24> rgb[9];
	sc_dt::sc_uint<24> o_rgb;

};
#endif
