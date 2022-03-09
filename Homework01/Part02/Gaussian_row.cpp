#include <cmath>

#include "Gaussian_row.h"

using std::cout;
using std::endl;

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
    counter = 0;
    for (unsigned int x = 0; x < 256; ++x){
        r_row_val[x] = 0;
        g_row_val[x] = 0;
        b_row_val[x] = 0;
    }
    while (true) {
        // Row buffer data transfer
        for (unsigned int y = 2; y > 0; --y){
            for (unsigned int x = 0; x < 256; ++x){
                r_row[y][x] = r_row[y-1][x] ;
                g_row[y][x] = g_row[y-1][x] ;
                b_row[y][x] = b_row[y-1][x] ;
            }
        }
        for (unsigned int x = 0; x < 256; ++x){
            r_row[0][x] = i_r[x].read() ;
            g_row[0][x] = i_g[x].read() ;
            b_row[0][x] = i_b[x].read() ;
        }

        // initiallize the row_val 
        for (unsigned int x = 0; x < 256; ++x){
            r_row_val[x] = 0;
            g_row_val[x] = 0;
            b_row_val[x] = 0;
        }

        if(counter > 0){
            for (unsigned int x = 0; x < 256; ++x){
                for (unsigned int v = 0; v <= 2; ++v){
                    for (unsigned int u = 0; u <= 2; ++u){
                        if(x == 0 && u == 0){
                            continue;
                        }else if(x == 255 && u == 2){
                            continue;
                        }else{
                            r_row_val[x] += r_row[v][x-1+u] * mask[0][u][v];
                            g_row_val[x] += g_row[v][x-1+u] * mask[0][u][v];
                            b_row_val[x] += b_row[v][x-1+u] * mask[0][u][v];
                        }
                        
                    }
                }
            }
            for (unsigned int x = 0; x < 256; ++x){
                o_r_result[x].write(r_row_val[x]);
                o_g_result[x].write(g_row_val[x]);
                o_b_result[x].write(b_row_val[x]);
            }

            wait(2);
        }else{
            wait(1);
        }
        counter++;
        cout << counter << endl;

    }
}
