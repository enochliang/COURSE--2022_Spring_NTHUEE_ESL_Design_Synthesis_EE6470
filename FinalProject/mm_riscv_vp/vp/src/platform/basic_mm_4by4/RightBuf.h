
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct RightBuf : public sc_module {
  tlm_utils::simple_target_socket<RightBuf> tsock;

  sc_fifo<char> i_0;
  sc_fifo<char> i_1;
  sc_fifo<char> i_2;
  sc_fifo<char> i_3;
  sc_fifo<char> o_0;
  sc_fifo<char> o_1;
  sc_fifo<char> o_2;
  sc_fifo<char> o_3;

  SC_HAS_PROCESS(RightBuf);

  RightBuf(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &RightBuf::blocking_transport);
    SC_THREAD(do_rbuf);
  }

  ~RightBuf() {
	}

  char IA_to_IA[4][4];
  unsigned int base_offset;

  void do_rbuf(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      for(int i = 0; i < 4; i++){
        IA_to_IA[i][0] = i_0.read();
        IA_to_IA[i][1] = i_1.read();
        IA_to_IA[i][2] = i_2.read();
        IA_to_IA[i][3] = i_3.read();
      }

      for(int i = 0; i < 4; i++){
        o_0.write(IA_to_IA[i][0]);
        o_1.write(IA_to_IA[i][1]);
        o_2.write(IA_to_IA[i][2]);
        o_3.write(IA_to_IA[i][3]);
        wait(CLOCK_PERIOD, SC_NS);
      }
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
          case RightBuf_RESULT_ADDR:
            buffer.uc[0] = o_0.read();
            buffer.uc[1] = o_1.read();
            buffer.uc[2] = o_2.read();
            buffer.uc[3] = o_3.read();
            break;
          default:
            std::cerr << "READ Error! RightBuf::blocking_transport: address 0x"
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
          case RightBuf_R_ADDR:
            i_0.write(data_ptr[0]);
            i_1.write(data_ptr[1]);
            i_2.write(data_ptr[2]);
            i_3.write(data_ptr[3]);
            break;
          default:
            std::cerr << "WRITE Error! RightBuf::blocking_transport: address 0x"
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
