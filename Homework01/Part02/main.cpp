#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "Gaussian_row.h"
#include "Testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {
  if ((argc < 3) || (argc > 4)) {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }

  //Create modules and signals
  Testbench tb("tb");
  GaussianFilter gaussian_filter("gaussian_filter");
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  //Create FIFO channels
  sc_fifo<unsigned char> r[256];
  sc_fifo<unsigned char> g[256];
  sc_fifo<unsigned char> b[256];
  sc_fifo<int> r_result[256];
  sc_fifo<int> g_result[256];
  sc_fifo<int> b_result[256];

  //Connect FIFO channels with modules
  tb.i_clk(clk);
  tb.o_rst(rst);
  gaussian_filter.i_clk(clk);
  gaussian_filter.i_rst(rst);
  for(int i = 0; i < 256; ++i){
    tb.o_r[i](r[i]);
    tb.o_g[i](g[i]);
    tb.o_b[i](b[i]);
    tb.i_r_result[i](r_result[i]);
    tb.i_g_result[i](g_result[i]);
    tb.i_b_result[i](b_result[i]);
    gaussian_filter.i_r[i](r[i]);
    gaussian_filter.i_g[i](g[i]);
    gaussian_filter.i_b[i](b[i]);
    gaussian_filter.o_r_result[i](r_result[i]);
    gaussian_filter.o_g_result[i](g_result[i]);
    gaussian_filter.o_b_result[i](b_result[i]);
  }
  
  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}
