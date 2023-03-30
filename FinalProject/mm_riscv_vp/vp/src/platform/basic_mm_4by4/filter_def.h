#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

#define CLOCK_PERIOD 10


#define MAX_IMAGE_BUFFER_LENTH 1024

// sobel mask parameters
const int DMA_TRANS = 64;

// Sobel Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int MatrixMult_R_ADDR = 0x00000000;
const int MatrixMult_RESULT_ADDR = 0x00000004;

const int MatrixMult00_RS_R_ADDR   = 0x00000000;
const int MatrixMult00_RS_W_WIDTH  = 0x00000004;
const int MatrixMult00_RS_W_HEIGHT = 0x00000008;
const int MatrixMult00_RS_W_DATA   = 0x0000000C;
const int MatrixMult00_RS_RESULT_ADDR = 0x00800000;

const int DownBuf_R_ADDR = 0x00000000;
const int DownBuf_RESULT_ADDR = 0x00000004;
const int RightBuf_R_ADDR = 0x00000000;
const int RightBuf_RESULT_ADDR = 0x00000004;


union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
