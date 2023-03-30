#include "string"
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};


char *W_bitmap;
char *I_bitmap;
char *O_bitmap;
unsigned char  buffer[4] = {0};

// Sobel Filter ACC
static char* const MatrixMult00_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MatrixMult01_START_ADDR = reinterpret_cast<char* const>(0x74000000);
static char* const MatrixMult02_START_ADDR = reinterpret_cast<char* const>(0x75000000);
static char* const MatrixMult03_START_ADDR = reinterpret_cast<char* const>(0x76000000);
static char* const MatrixMult10_START_ADDR = reinterpret_cast<char* const>(0x77000000);
static char* const MatrixMult11_START_ADDR = reinterpret_cast<char* const>(0x78000000);
static char* const MatrixMult12_START_ADDR = reinterpret_cast<char* const>(0x79000000);
static char* const MatrixMult13_START_ADDR = reinterpret_cast<char* const>(0x7a000000);
static char* const MatrixMult20_START_ADDR = reinterpret_cast<char* const>(0x7b000000);
static char* const MatrixMult21_START_ADDR = reinterpret_cast<char* const>(0x7c000000);
static char* const MatrixMult22_START_ADDR = reinterpret_cast<char* const>(0x7d000000);
static char* const MatrixMult23_START_ADDR = reinterpret_cast<char* const>(0x7e000000);
static char* const MatrixMult30_START_ADDR = reinterpret_cast<char* const>(0x7f000000);
static char* const MatrixMult31_START_ADDR = reinterpret_cast<char* const>(0x80000000);
static char* const MatrixMult32_START_ADDR = reinterpret_cast<char* const>(0x81000000);
static char* const MatrixMult33_START_ADDR = reinterpret_cast<char* const>(0x82000000);
static char* const MatrixMult00_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);
static char* const MatrixMult01_READ_ADDR  = reinterpret_cast<char* const>(0x74000004);
static char* const MatrixMult02_READ_ADDR  = reinterpret_cast<char* const>(0x75000004);
static char* const MatrixMult03_READ_ADDR  = reinterpret_cast<char* const>(0x76000004);
static char* const MatrixMult10_READ_ADDR  = reinterpret_cast<char* const>(0x77000004);
static char* const MatrixMult11_READ_ADDR  = reinterpret_cast<char* const>(0x78000004);
static char* const MatrixMult12_READ_ADDR  = reinterpret_cast<char* const>(0x79000004);
static char* const MatrixMult13_READ_ADDR  = reinterpret_cast<char* const>(0x7a000004);
static char* const MatrixMult20_READ_ADDR  = reinterpret_cast<char* const>(0x7b000004);
static char* const MatrixMult21_READ_ADDR  = reinterpret_cast<char* const>(0x7c000004);
static char* const MatrixMult22_READ_ADDR  = reinterpret_cast<char* const>(0x7d000004);
static char* const MatrixMult23_READ_ADDR  = reinterpret_cast<char* const>(0x7e000004);
static char* const MatrixMult30_READ_ADDR  = reinterpret_cast<char* const>(0x7f000004);
static char* const MatrixMult31_READ_ADDR  = reinterpret_cast<char* const>(0x80000004);
static char* const MatrixMult32_READ_ADDR  = reinterpret_cast<char* const>(0x81000004);
static char* const MatrixMult33_READ_ADDR  = reinterpret_cast<char* const>(0x82000004);


static char* const DownBuf_00_to_10_START_ADDR = reinterpret_cast<char* const>(0x83000000);
static char* const DownBuf_10_to_20_START_ADDR = reinterpret_cast<char* const>(0x83001000);
static char* const DownBuf_20_to_30_START_ADDR = reinterpret_cast<char* const>(0x83002000);
static char* const DownBuf_01_to_11_START_ADDR = reinterpret_cast<char* const>(0x83003000);
static char* const DownBuf_11_to_21_START_ADDR = reinterpret_cast<char* const>(0x83004000);
static char* const DownBuf_21_to_31_START_ADDR = reinterpret_cast<char* const>(0x83005000);
static char* const DownBuf_02_to_12_START_ADDR = reinterpret_cast<char* const>(0x83006000);
static char* const DownBuf_12_to_22_START_ADDR = reinterpret_cast<char* const>(0x83007000);
static char* const DownBuf_22_to_32_START_ADDR = reinterpret_cast<char* const>(0x83008000);
static char* const DownBuf_03_to_13_START_ADDR = reinterpret_cast<char* const>(0x83009000);
static char* const DownBuf_13_to_23_START_ADDR = reinterpret_cast<char* const>(0x8300a000);
static char* const DownBuf_23_to_33_START_ADDR = reinterpret_cast<char* const>(0x8300b000);
static char* const RightBuf_00_to_01_START_ADDR = reinterpret_cast<char* const>(0x8300c000);
static char* const RightBuf_01_to_02_START_ADDR = reinterpret_cast<char* const>(0x8300d000);
static char* const RightBuf_02_to_03_START_ADDR = reinterpret_cast<char* const>(0x8300e000);
static char* const RightBuf_10_to_11_START_ADDR = reinterpret_cast<char* const>(0x8300f000);
static char* const RightBuf_11_to_12_START_ADDR = reinterpret_cast<char* const>(0x83010000);
static char* const RightBuf_12_to_13_START_ADDR = reinterpret_cast<char* const>(0x83011000);
static char* const RightBuf_20_to_21_START_ADDR = reinterpret_cast<char* const>(0x83012000);
static char* const RightBuf_21_to_22_START_ADDR = reinterpret_cast<char* const>(0x83013000);
static char* const RightBuf_22_to_23_START_ADDR = reinterpret_cast<char* const>(0x83014000);
static char* const RightBuf_30_to_31_START_ADDR = reinterpret_cast<char* const>(0x83015000);
static char* const RightBuf_31_to_32_START_ADDR = reinterpret_cast<char* const>(0x83016000);
static char* const RightBuf_32_to_33_START_ADDR = reinterpret_cast<char* const>(0x83017000);

static char* const DownBuf_00_to_10_READ_ADDR = reinterpret_cast<char* const>(0x83000004);
static char* const DownBuf_10_to_20_READ_ADDR = reinterpret_cast<char* const>(0x83001004);
static char* const DownBuf_20_to_30_READ_ADDR = reinterpret_cast<char* const>(0x83002004);
static char* const DownBuf_01_to_11_READ_ADDR = reinterpret_cast<char* const>(0x83003004);
static char* const DownBuf_11_to_21_READ_ADDR = reinterpret_cast<char* const>(0x83004004);
static char* const DownBuf_21_to_31_READ_ADDR = reinterpret_cast<char* const>(0x83005004);
static char* const DownBuf_02_to_12_READ_ADDR = reinterpret_cast<char* const>(0x83006004);
static char* const DownBuf_12_to_22_READ_ADDR = reinterpret_cast<char* const>(0x83007004);
static char* const DownBuf_22_to_32_READ_ADDR = reinterpret_cast<char* const>(0x83008004);
static char* const DownBuf_03_to_13_READ_ADDR = reinterpret_cast<char* const>(0x83009004);
static char* const DownBuf_13_to_23_READ_ADDR = reinterpret_cast<char* const>(0x8300a004);
static char* const DownBuf_23_to_33_READ_ADDR = reinterpret_cast<char* const>(0x8300b004);
static char* const RightBuf_00_to_01_READ_ADDR = reinterpret_cast<char* const>(0x8300c004);
static char* const RightBuf_01_to_02_READ_ADDR = reinterpret_cast<char* const>(0x8300d004);
static char* const RightBuf_02_to_03_READ_ADDR = reinterpret_cast<char* const>(0x8300e004);
static char* const RightBuf_10_to_11_READ_ADDR = reinterpret_cast<char* const>(0x8300f004);
static char* const RightBuf_11_to_12_READ_ADDR = reinterpret_cast<char* const>(0x83010004);
static char* const RightBuf_12_to_13_READ_ADDR = reinterpret_cast<char* const>(0x83011004);
static char* const RightBuf_20_to_21_READ_ADDR = reinterpret_cast<char* const>(0x83012004);
static char* const RightBuf_21_to_22_READ_ADDR = reinterpret_cast<char* const>(0x83013004);
static char* const RightBuf_22_to_23_READ_ADDR = reinterpret_cast<char* const>(0x83014004);
static char* const RightBuf_30_to_31_READ_ADDR = reinterpret_cast<char* const>(0x83015004);
static char* const RightBuf_31_to_32_READ_ADDR = reinterpret_cast<char* const>(0x83016004);
static char* const RightBuf_32_to_33_READ_ADDR = reinterpret_cast<char* const>(0x83017004);



// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;
int read_mat(std::string infile_name) {
  FILE *fp_s = NULL;
  fp_s = fopen(infile_name.c_str(), "rb");
  
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }

  W_bitmap =
      (char *)malloc((size_t)256);
  if (W_bitmap == NULL) {
    printf("malloc W error\n");
    return -1;
  }
  
  I_bitmap =
      (char *)malloc((size_t)256);
  if (I_bitmap == NULL) {
    printf("malloc I error\n");
    return -1;
  }

  O_bitmap =
      (char *)malloc((size_t)256);
  if (O_bitmap == NULL) {
    printf("malloc O error\n");
    return -1;
  }

  int tt;
  for (int i = 0; i < 16; i++){
    for (int j = 0; j < 16; j++){
      fscanf(fp_s,"%d",&tt);
      W_bitmap[16*i+j]=(char)tt;
    }
  }

  for (int i = 0; i < 16; i++){
    for (int j = 0; j < 16; j++){
      fscanf(fp_s,"%d",&tt);
      I_bitmap[16*i+j]=(char)tt;
    }
  }

  fclose(fp_s);

  return 0;
}

int write_mat(std::string outfile_name) {
  FILE *fp_t = NULL; // target file handler

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }

  for (int i = 0; i < 16; i++){
    for (int j = 0; j < 16; j++){
      fprintf(fp_t,"%d ",*(O_bitmap + (16*i) + j));
    }
    fprintf(fp_t,"\n");
  }

  fclose(fp_t);
  return 0;
}

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}
void trans_data_betweem_ACC(char* ADDR, char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}

void transfer_in(int PE_num,bool R0_D1){
  switch(PE_num){
    case 0:
      write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
    break;
    case 1:
      trans_data_betweem_ACC(MatrixMult01_START_ADDR, RightBuf_00_to_01_READ_ADDR, 4);
    break;
    case 2:
      trans_data_betweem_ACC(MatrixMult02_START_ADDR, RightBuf_01_to_02_READ_ADDR, 4);
    break;
    case 3:
      trans_data_betweem_ACC(MatrixMult03_START_ADDR, RightBuf_02_to_03_READ_ADDR, 4);
    break;
    case 4:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult10_START_ADDR, DownBuf_00_to_10_READ_ADDR, 4);
      }else{
        write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
      }
    break;
    case 5:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult11_START_ADDR, DownBuf_01_to_11_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult11_START_ADDR, RightBuf_10_to_11_READ_ADDR, 4);
      }
    break;
    case 6:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult12_START_ADDR, DownBuf_02_to_12_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult12_START_ADDR, RightBuf_11_to_12_READ_ADDR, 4);
      }
    break;
    case 7:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult13_START_ADDR, DownBuf_03_to_13_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult13_START_ADDR, RightBuf_12_to_13_READ_ADDR, 4);
      }
    break;
    case 8:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult20_START_ADDR, DownBuf_10_to_20_READ_ADDR, 4);
      }else{
        write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
      }
    break;
    case 9:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult21_START_ADDR, DownBuf_11_to_21_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult21_START_ADDR, RightBuf_20_to_21_READ_ADDR, 4);
      }
    break;
    case 10:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult22_START_ADDR, DownBuf_12_to_22_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult22_START_ADDR, RightBuf_21_to_22_READ_ADDR, 4);
      }
    break;
    case 11:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult23_START_ADDR, DownBuf_13_to_23_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult23_START_ADDR, RightBuf_22_to_23_READ_ADDR, 4);
      }
    break;
    case 12:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult30_START_ADDR, DownBuf_20_to_30_READ_ADDR, 4);
      }else{
        write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
      }
    break;
    case 13:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult31_START_ADDR, DownBuf_21_to_31_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult31_START_ADDR, RightBuf_30_to_31_READ_ADDR, 4);
      }
    break;
    case 14:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult32_START_ADDR, DownBuf_22_to_32_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult32_START_ADDR, RightBuf_31_to_32_READ_ADDR, 4);
      }
    break;
    case 15:
      if(R0_D1){
        trans_data_betweem_ACC(MatrixMult33_START_ADDR, DownBuf_23_to_33_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(MatrixMult33_START_ADDR, RightBuf_32_to_33_READ_ADDR, 4);
      }
    break;
  }
}

void transfer_out(int PE_num,bool R0_D1){
  switch(PE_num){
    case 0:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_00_to_10_START_ADDR, MatrixMult00_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_00_to_01_START_ADDR, MatrixMult00_READ_ADDR, 4);
      }
    break;
    case 1:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_01_to_11_START_ADDR, MatrixMult01_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_01_to_02_START_ADDR, MatrixMult01_READ_ADDR, 4);
      }
    break;
    case 2:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_02_to_12_START_ADDR, MatrixMult02_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_02_to_03_START_ADDR, MatrixMult02_READ_ADDR, 4);
      }
    break;
    case 3:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_03_to_13_START_ADDR, MatrixMult03_READ_ADDR, 4);
      }else{
        read_data_from_ACC(MatrixMult03_READ_ADDR, buffer, 4);
      }
    break;
    case 4:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_10_to_20_START_ADDR, MatrixMult10_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_10_to_11_START_ADDR, MatrixMult10_READ_ADDR, 4);
      }
    break;
    case 5:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_11_to_21_START_ADDR, MatrixMult11_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_11_to_12_START_ADDR, MatrixMult11_READ_ADDR, 4);
      }
    break;
    case 6:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_12_to_22_START_ADDR, MatrixMult12_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_12_to_13_START_ADDR, MatrixMult12_READ_ADDR, 4);
      }
    break;
    case 7:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_13_to_23_START_ADDR, MatrixMult13_READ_ADDR, 4);
      }else{
        read_data_from_ACC(MatrixMult13_READ_ADDR, buffer, 4);
      }
    break;
    case 8:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_20_to_30_START_ADDR, MatrixMult20_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_20_to_21_START_ADDR, MatrixMult20_READ_ADDR, 4);
      }
    break;
    case 9:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_21_to_31_START_ADDR, MatrixMult21_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_21_to_22_START_ADDR, MatrixMult21_READ_ADDR, 4);
      }
    break;
    case 10:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_22_to_32_START_ADDR, MatrixMult22_READ_ADDR, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_22_to_23_START_ADDR, MatrixMult22_READ_ADDR, 4);
      }
    break;
    case 11:
      if(R0_D1){
        trans_data_betweem_ACC(DownBuf_23_to_33_START_ADDR, MatrixMult23_READ_ADDR, 4);
      }else{
        read_data_from_ACC(MatrixMult23_READ_ADDR, buffer, 4);
      }
    break;
    case 12:
      if(R0_D1){
        read_data_from_ACC(MatrixMult30_READ_ADDR, buffer, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_30_to_31_START_ADDR, MatrixMult30_READ_ADDR, 4);
      }
    break;
    case 13:
      if(R0_D1){
        read_data_from_ACC(MatrixMult31_READ_ADDR, buffer, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_31_to_32_START_ADDR, MatrixMult31_READ_ADDR, 4);
      }
    break;
    case 14:
      if(R0_D1){
        read_data_from_ACC(MatrixMult32_READ_ADDR, buffer, 4);
      }else{
        trans_data_betweem_ACC(RightBuf_32_to_33_START_ADDR, MatrixMult32_READ_ADDR, 4);
      }
    break;
    case 15:
      read_data_from_ACC(MatrixMult33_READ_ADDR, buffer, 4);
    break;
  }
}

int main(int argc, char *argv[]) {
  read_mat("input.txt");
  
  word data;
  int total;

  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      for(int u = 0; u < 4; u ++){//write W submatrix
        buffer[0] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 0);
        buffer[1] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 1);
        buffer[2] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 2);
        buffer[3] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 3);
        switch(4*i+j){
          case 0:
            write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
          break;
          case 1:
            write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
          break;
          case 2:
            write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
          break;
          case 3:
            write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
          break;
          case 4:
            write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
          break;
          case 5:
            write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
          break;
          case 6:
            write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
          break;
          case 7:
            write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
          break;
          case 8:
            write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
          break;
          case 9:
            write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
          break;
          case 10:
            write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
          break;
          case 11:
            write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
          break;
          case 12:
            write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
          break;
          case 13:
            write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);
          break;
          case 14:
            write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);
          break;
          case 15:
            write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);
          break;
        }
      }
    }
  }
  
  // 2 x x x
  // x x x x
  // x x x x 
  // x x x x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
  for(int u = 0; u < 4; u ++){//write W submatrix
    buffer[0] = *(I_bitmap + (16*u) + 0);
    buffer[1] = *(I_bitmap + (16*u) + 1);
    buffer[2] = *(I_bitmap + (16*u) + 2);
    buffer[3] = *(I_bitmap + (16*u) + 3);
    transfer_in(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(0,1);
  }

  // 2 2 x x
  // 1 x x x
  // x x x x 
  // x x x x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*1) + (4*0) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*1) + (4*0) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*1) + (4*0) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*1) + (4*0) + (16*u) + 3);
    transfer_in(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(1,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*0) + (4*1) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*0) + (4*1) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*0) + (4*1) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*0) + (4*1) + (16*u) + 3);
    transfer_in(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(1,1);
  }
  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(0,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,1);
  }

  // 2 2 2 x
  // 1 1 x x
  // 1 x x x 
  // x x x x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*2) + (4*0) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*2) + (4*0) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*2) + (4*0) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*2) + (4*0) + (16*u) + 3);
    transfer_in(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(2,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*1) + (4*1) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*1) + (4*1) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*1) + (4*1) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*1) + (4*1) + (16*u) + 3);
    transfer_in(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*0) + (4*2) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*0) + (4*2) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*0) + (4*2) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*0) + (4*2) + (16*u) + 3);
    transfer_in(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(8,1);
  }
  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(0,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,1);
  }

  // 2 2 2 2
  // 1 1 1 x
  // 1 1 x x 
  // 1 x x x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);

  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*3) + (4*0) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*3) + (4*0) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*3) + (4*0) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*3) + (4*0) + (16*u) + 3);
    transfer_in(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(3,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*2) + (4*1) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*2) + (4*1) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*2) + (4*1) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*2) + (4*1) + (16*u) + 3);
    transfer_in(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*1) + (4*2) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*1) + (4*2) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*1) + (4*2) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*1) + (4*2) + (16*u) + 3);
    transfer_in(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*0) + (4*3) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*0) + (4*3) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*0) + (4*3) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*0) + (4*3) + (16*u) + 3);
    transfer_in(12,0);
  }
  
  for(int u = 0; u < 4; u ++){
    transfer_in(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(12,1);
  }
  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(0,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_out(0,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(1,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,1);
    *(O_bitmap + (64*0) + (4*0) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*0) + (4*0) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*0) + (4*0) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*0) + (4*0) + (16*u) + 3) = buffer[3];
  }

  // x 2 2 2
  // 1 1 1 1
  // 1 1 1 x 
  // 1 1 x x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult01_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult10_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);

  for(int u = 0; u < 4; u ++){
    transfer_in(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(3,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*3) + (4*1) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*3) + (4*1) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*3) + (4*1) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*3) + (4*1) + (16*u) + 3);
    transfer_in(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*2) + (4*2) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*2) + (4*2) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*2) + (4*2) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*2) + (4*2) + (16*u) + 3);
    transfer_in(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*1) + (4*3) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*1) + (4*3) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*1) + (4*3) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*1) + (4*3) + (16*u) + 3);
    transfer_in(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(12,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(1,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_out(1,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(2,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(4,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,1);
    *(O_bitmap + (64*1) + (4*0) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*1) + (4*0) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*1) + (4*0) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*1) + (4*0) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,1);
    *(O_bitmap + (64*0) + (4*1) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*0) + (4*1) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*0) + (4*1) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*0) + (4*1) + (16*u) + 3) = buffer[3];
  }

  // x x 2 2
  // x 1 1 1
  // 1 1 1 1 
  // 1 1 1 x
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult02_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult11_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult20_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);


  for(int u = 0; u < 4; u ++){
    transfer_in(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*3) + (4*2) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*3) + (4*2) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*3) + (4*2) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*3) + (4*2) + (16*u) + 3);
    transfer_in(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*2) + (4*3) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*2) + (4*3) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*2) + (4*3) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*2) + (4*3) + (16*u) + 3);
    transfer_in(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(12,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(2,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_out(2,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(3,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(5,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(8,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,1);
    *(O_bitmap + (64*2) + (4*0) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*2) + (4*0) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*2) + (4*0) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*2) + (4*0) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,1);
    *(O_bitmap + (64*1) + (4*1) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*1) + (4*1) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*1) + (4*1) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*1) + (4*1) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,1);
    *(O_bitmap + (64*0) + (4*2) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*0) + (4*2) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*0) + (4*2) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*0) + (4*2) + (16*u) + 3) = buffer[3];
  }

  // x x x 2
  // x x 1 1
  // x 1 1 1 
  // 1 1 1 1
  buffer[0] = 2;
  write_data_to_ACC(MatrixMult03_START_ADDR, buffer, 4);
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult12_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult21_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult30_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);

  
  for(int u = 0; u < 4; u ++){
    transfer_in(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,0);
  }
  for(int u = 0; u < 4; u ++){
    buffer[0] = *(I_bitmap + (64*3) + (4*3) + (16*u) + 0);
    buffer[1] = *(I_bitmap + (64*3) + (4*3) + (16*u) + 1);
    buffer[2] = *(I_bitmap + (64*3) + (4*3) + (16*u) + 2);
    buffer[3] = *(I_bitmap + (64*3) + (4*3) + (16*u) + 3);
    transfer_in(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_in(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(12,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(3,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_out(3,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(6,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(9,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(12,1);
    *(O_bitmap + (64*3) + (4*0) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*3) + (4*0) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*3) + (4*0) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*3) + (4*0) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,1);
    *(O_bitmap + (64*2) + (4*1) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*2) + (4*1) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*2) + (4*1) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*2) + (4*1) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,1);
    *(O_bitmap + (64*1) + (4*2) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*1) + (4*2) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*1) + (4*2) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*1) + (4*2) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,1);
    *(O_bitmap + (64*0) + (4*3) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*0) + (4*3) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*0) + (4*3) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*0) + (4*3) + (16*u) + 3) = buffer[3];
  }

  // x x x x
  // x x x 1
  // x x 1 1 
  // x 1 1 1
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult13_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult22_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult31_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);

  

  for(int u = 0; u < 4; u ++){
    transfer_in(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_in(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(13,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(7,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,0);
  }

  
  for(int u = 0; u < 4; u ++){
    transfer_out(7,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(10,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(13,1);
    *(O_bitmap + (64*3) + (4*1) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*3) + (4*1) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*3) + (4*1) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*3) + (4*1) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,1);
    *(O_bitmap + (64*2) + (4*2) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*2) + (4*2) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*2) + (4*2) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*2) + (4*2) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,1);
    *(O_bitmap + (64*1) + (4*3) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*1) + (4*3) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*1) + (4*3) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*1) + (4*3) + (16*u) + 3) = buffer[3];
  }

  // x x x x
  // x x x x
  // x x x 1 
  // x x 1 1
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult23_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult32_START_ADDR, buffer, 4);
  write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);

  

  for(int u = 0; u < 4; u ++){
    transfer_in(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_in(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(14,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_in(15,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(11,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,0);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,0);
  }

  
  for(int u = 0; u < 4; u ++){
    transfer_out(11,1);
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(14,1);
    *(O_bitmap + (64*3) + (4*2) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*3) + (4*2) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*3) + (4*2) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*3) + (4*2) + (16*u) + 3) = buffer[3];
  }
  for(int u = 0; u < 4; u ++){
    transfer_out(15,1);
    *(O_bitmap + (64*2) + (4*3) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*2) + (4*3) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*2) + (4*3) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*2) + (4*3) + (16*u) + 3) = buffer[3];
  }

  // x x x x
  // x x x x
  // x x x x 
  // x x x 1
  buffer[0] = 1;
  write_data_to_ACC(MatrixMult33_START_ADDR, buffer, 4);

  for(int u = 0; u < 4; u ++){
    transfer_in(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_in(15,1);
  }

  //==
  for(int u = 0; u < 4; u ++){
    transfer_out(15,0);
  }

  for(int u = 0; u < 4; u ++){
    transfer_out(15,1);
    *(O_bitmap + (64*3) + (4*3) + (16*u) + 0) = buffer[0];
    *(O_bitmap + (64*3) + (4*3) + (16*u) + 1) = buffer[1];
    *(O_bitmap + (64*3) + (4*3) + (16*u) + 2) = buffer[2];
    *(O_bitmap + (64*3) + (4*3) + (16*u) + 3) = buffer[3];
  }

  write_mat("output.txt");
}

