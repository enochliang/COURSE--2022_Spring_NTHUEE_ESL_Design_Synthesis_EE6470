#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct MatrixMult : public sc_module {
  tlm_utils::simple_target_socket<MatrixMult> tsock;

  sc_fifo<char> i_0;
  sc_fifo<char> i_1;
  sc_fifo<char> i_2;
  sc_fifo<char> i_3;
  sc_fifo<char> o_0;
  sc_fifo<char> o_1;
  sc_fifo<char> o_2;
  sc_fifo<char> o_3;

  SC_HAS_PROCESS(MatrixMult);

  MatrixMult(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &MatrixMult::blocking_transport);
    SC_THREAD(do_mm);
  }

  ~MatrixMult() {
	}

  char W[4][4];
  char IA[4][4];
  char OA[4][4];
  char B[4][4];
  unsigned char mode;
  unsigned int base_offset;

  void do_mm(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      mode = i_0.read();
      i_1.read();
      i_2.read();
      i_3.read();
      wait(CLOCK_PERIOD, SC_NS);

      switch(mode){
        case 0:
          for(int i = 0; i < 4; i++){
            W[i][0] = i_0.read();
            W[i][1] = i_1.read();
            W[i][2] = i_2.read();
            W[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
        break;
        case 1:
          for(int i = 0; i < 4; i++){
            IA[i][0] = i_0.read();
            IA[i][1] = i_1.read();
            IA[i][2] = i_2.read();
            IA[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            B[i][0] = i_0.read();
            B[i][1] = i_1.read();
            B[i][2] = i_2.read();
            B[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int x = 0; x < 4; x++ ){
            for(int y = 0; y < 4; y++ ){
              OA[x][y] = 0;
              for(int i = 0; i < 4; i++){
                OA[x][y] += W[x][i]*IA[i][y];
              }
              OA[x][y] += B[x][y];
            }
            wait(CLOCK_PERIOD, SC_NS);
          }

          for(int i = 0; i < 4; i++){
            o_0.write(IA[i][0]);
            o_1.write(IA[i][1]);
            o_2.write(IA[i][2]);
            o_3.write(IA[i][3]);
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            o_0.write(OA[i][0]);
            o_1.write(OA[i][1]);
            o_2.write(OA[i][2]);
            o_3.write(OA[i][3]);
            wait(CLOCK_PERIOD, SC_NS);
          }
        break;
        case 2:
          for(int i = 0; i < 4; i++){
            IA[i][0] = i_0.read();
            IA[i][1] = i_1.read();
            IA[i][2] = i_2.read();
            IA[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int x = 0; x < 4; x++ ){
            for(int y = 0; y < 4; y++ ){
              OA[x][y] = 0;
              for(int i = 0; i < 4; i++){
                OA[x][y] += W[x][i]*IA[i][y];
              }
            }
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            o_0.write(IA[i][0]);
            o_1.write(IA[i][1]);
            o_2.write(IA[i][2]);
            o_3.write(IA[i][3]);
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            o_0.write(OA[i][0]);
            o_1.write(OA[i][1]);
            o_2.write(OA[i][2]);
            o_3.write(OA[i][3]);
            wait(CLOCK_PERIOD, SC_NS);
          }

        break;
        case 3:
          for(int i = 0; i < 4; i++){
            IA[i][0] = i_0.read();
            IA[i][1] = i_1.read();
            IA[i][2] = i_2.read();
            IA[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            B[i][0] = i_0.read();
            B[i][1] = i_1.read();
            B[i][2] = i_2.read();
            B[i][3] = i_3.read();
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int x = 0; x < 4; x++ ){
            for(int y = 0; y < 4; y++ ){
              OA[x][y] = IA[x][y] + B[x][y];
            }
            wait(CLOCK_PERIOD, SC_NS);
          }
          for(int i = 0; i < 4; i++){
            o_0.write(OA[i][0]);
            o_1.write(OA[i][1]);
            o_2.write(OA[i][2]);
            o_3.write(OA[i][3]);
            wait(CLOCK_PERIOD, SC_NS);
          }
        break;
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
          case MatrixMult_RESULT_ADDR:
            buffer.uc[0] = o_0.read();
            buffer.uc[1] = o_1.read();
            buffer.uc[2] = o_2.read();
            buffer.uc[3] = o_3.read();
            break;
          default:
            std::cerr << "READ Error! MatrixMult::blocking_transport: address 0x"
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
          case MatrixMult_R_ADDR:
            i_0.write(data_ptr[0]);
            i_1.write(data_ptr[1]);
            i_2.write(data_ptr[2]);
            i_3.write(data_ptr[3]);
            break;
          default:
            std::cerr << "WRITE Error! MatrixMult::blocking_transport: address 0x"
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
