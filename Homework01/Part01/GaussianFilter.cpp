#include <cmath>

#include "GaussianFilter.h"

GaussianFilter::GaussianFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// gaussian mask
const double mask[MASK_N][MASK_X][MASK_Y] =
{
  0.077847, 0.123317, 0.077847,
  0.123317, 0.195346, 0.123317,
  0.077847, 0.123317, 0.077847,
};


void GaussianFilter::do_filter() {
  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) {
      r_val[i] = 0;
      g_val[i] = 0;
      b_val[i] = 0;
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        for (unsigned int i = 0; i != MASK_N; ++i) {
          //val[i] += grey * mask[i][u][v];
          r_val[i] += i_r.read() * mask[i][u][v];
          g_val[i] += i_g.read() * mask[i][u][v];
          b_val[i] += i_b.read() * mask[i][u][v];
        }
      }
    }
    double r_total = 0;
    double g_total = 0;
    double b_total = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      r_total += r_val[i] * r_val[i];
      g_total += g_val[i] * g_val[i];
      b_total += b_val[i] * b_val[i];
    }
    int r_result = (int)(std::sqrt(r_total));
    int g_result = (int)(std::sqrt(g_total));
    int b_result = (int)(std::sqrt(b_total));
    o_r_result.write(r_result);
    o_g_result.write(g_result);
    o_b_result.write(b_result);
    wait(10); //emulate module delay
  }
}
