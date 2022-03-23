#include <cmath>
#include <iomanip>

#include "GaussianFilter.h"

GaussianFilter::GaussianFilter(sc_module_name n) : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_filter);

  t_skt.register_b_transport(this, &GaussianFilter::blocking_transport);
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
    unsigned int r_result = (int)(std::sqrt(r_total));
    unsigned int g_result = (int)(std::sqrt(g_total));
    unsigned int b_result = (int)(std::sqrt(b_total));
    o_r_result.write((unsigned char)r_result);
    o_g_result.write((unsigned char)g_result);
    o_b_result.write((unsigned char)b_result);
    //wait(10); //emulate module delay
  }
}

void GaussianFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                        sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_RESULT_ADDR:
      buffer.uc[0] = o_r_result.read();
      buffer.uc[1] = o_g_result.read();
      buffer.uc[2] = o_b_result.read();
      buffer.uc[3] = 0;
    break;
    case SOBEL_FILTER_CHECK_ADDR:
      buffer.uint = o_r_result.num_available();
      buffer.uint += o_g_result.num_available();
      buffer.uint += o_b_result.num_available();
    break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
      }
      break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
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