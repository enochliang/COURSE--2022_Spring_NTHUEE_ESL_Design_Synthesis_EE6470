#ifndef GAUSSIAN_ROW_H_
#define GAUSSIAN_ROW_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class GaussianFilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r[256];
  sc_fifo_in<unsigned char> i_g[256];
  sc_fifo_in<unsigned char> i_b[256];
  sc_fifo_out<int> o_r_result[256];
  sc_fifo_out<int> o_g_result[256];
  sc_fifo_out<int> o_b_result[256];

  SC_HAS_PROCESS(GaussianFilter);
  GaussianFilter(sc_module_name n);
  ~GaussianFilter() = default;

private:
  void do_filter();
  int r_row_val[256];
  int g_row_val[256];
  int b_row_val[256];

  unsigned char r_row[3][256];
  unsigned char g_row[3][256];
  unsigned char b_row[3][256];

  int counter;
};
#endif
