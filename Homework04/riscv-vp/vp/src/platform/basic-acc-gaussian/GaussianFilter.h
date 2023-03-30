#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct GaussianFilter : public sc_module {
  tlm_utils::simple_target_socket<GaussianFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> o_r_result;
  sc_fifo<unsigned char> o_g_result;
  sc_fifo<unsigned char> o_b_result;

  SC_HAS_PROCESS(GaussianFilter);

  GaussianFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &GaussianFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~GaussianFilter() {
	}

  int r_val[MASK_N];
  int g_val[MASK_N];
  int b_val[MASK_N];
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      for (unsigned int i = 0; i < MASK_N; ++i) {
        r_val[i] = 0;
        g_val[i] = 0;
        b_val[i] = 0;
        wait(CLOCK_PERIOD, SC_NS);
      }
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          unsigned char red = i_r.read();
          unsigned char green = i_g.read();
          unsigned char blue = i_b.read();
          wait(CLOCK_PERIOD, SC_NS);
          for (unsigned int i = 0; i != MASK_N; ++i) {
            r_val[i] += red * mask[i][u][v];
            g_val[i] += green * mask[i][u][v];
            b_val[i] += blue * mask[i][u][v];
            wait(CLOCK_PERIOD, SC_NS);
          }
        }
      }
      unsigned char r_total = 0;
      unsigned char g_total = 0;
      unsigned char b_total = 0;
      for (unsigned int i = 0; i != MASK_N; ++i) {
        r_total = r_val[i]/16;
        g_total = g_val[i]/16;
        b_total = b_val[i]/16;
        wait(CLOCK_PERIOD, SC_NS);
      }
      // cout << (int)result << endl;

      o_r_result.write(r_total);
      o_g_result.write(g_total);
      o_b_result.write(b_total);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case Gaussian_FILTER_RESULT_ADDR:
            buffer.uc[0] = o_r_result.read();
            buffer.uc[1] = o_g_result.read();
            buffer.uc[2] = o_b_result.read();
            buffer.uc[3] = 0;
            break;
          default:
            std::cerr << "READ Error! GaussianFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case Gaussian_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! GaussianFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
