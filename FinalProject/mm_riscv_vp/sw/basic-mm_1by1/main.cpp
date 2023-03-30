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
char *Tmp_bitmap;

// Sobel Filter ACC
static char* const MatrixMult00_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MatrixMult00_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);

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

  Tmp_bitmap =
      (char *)malloc((size_t)1024);
  if (Tmp_bitmap == NULL) {
    printf("malloc Tmp error\n");
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

int main(int argc, char *argv[]) {
  unsigned char  buffer[4] = {0};
  word data;
  int total;

  read_mat("input.txt");
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      //printf("pixel (%d, %d); \n", i, j);
      buffer[0] = 0;
      buffer[1] = 0;
      buffer[2] = 0;
      buffer[3] = 0;
      write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);

      for(int u = 0; u < 4; u ++){//write W submatrix
        buffer[0] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 0);
        buffer[1] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 1);
        buffer[2] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 2);
        buffer[3] = *(W_bitmap + (64*i) + (4*j) + (16*u) + 3);
        write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
      }

      for(int z = 0; z < 4; z++){//change IA submatrix by column
        buffer[0] = 2;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 0;
        write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);

        for(int u = 0; u < 4; u ++){//send IA submatrix by 4 cycles
          buffer[0] = *(I_bitmap + (64*i) + (4*z) + (16*u) + 0);
          buffer[1] = *(I_bitmap + (64*i) + (4*z) + (16*u) + 1);
          buffer[2] = *(I_bitmap + (64*i) + (4*z) + (16*u) + 2);
          buffer[3] = *(I_bitmap + (64*i) + (4*z) + (16*u) + 3);
          write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
        }

        for(int u = 0; u < 4; u ++){// read IA submatrix by 4 cycles (no use)
          read_data_from_ACC(MatrixMult00_READ_ADDR, buffer, 4);
        }

        for(int u = 0; u < 4; u ++){//read Partial Matrix
          read_data_from_ACC(MatrixMult00_READ_ADDR, buffer, 4);//64 bytes per row
          *(Tmp_bitmap + (256*i) + (4*j) + (16*z) + (64*u) + 0) = buffer[0];
          *(Tmp_bitmap + (256*i) + (4*j) + (16*z) + (64*u) + 1) = buffer[1];
          *(Tmp_bitmap + (256*i) + (4*j) + (16*z) + (64*u) + 2) = buffer[2];
          *(Tmp_bitmap + (256*i) + (4*j) + (16*z) + (64*u) + 3) = buffer[3];
        }
      }
    }
  }

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      for(int z = 1; z < 4; z ++){
        //write mode
        buffer[0] = 3;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 0;
        write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
        for(int u = 0; u < 4; u ++){
          buffer[0] = *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 0);
          buffer[1] = *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 1);
          buffer[2] = *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 2);
          buffer[3] = *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 3);
          write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
        }

        for(int u = 0; u < 4; u ++){
          buffer[0] = *(Tmp_bitmap + (256*i) + (4*z) + (16*j) + (64*u) + 0);
          buffer[1] = *(Tmp_bitmap + (256*i) + (4*z) + (16*j) + (64*u) + 1);
          buffer[2] = *(Tmp_bitmap + (256*i) + (4*z) + (16*j) + (64*u) + 2);
          buffer[3] = *(Tmp_bitmap + (256*i) + (4*z) + (16*j) + (64*u) + 3);
          write_data_to_ACC(MatrixMult00_START_ADDR, buffer, 4);
        }

        if(z==3){
          for(int u = 0; u < 4; u ++){//read Partial Matrix
            read_data_from_ACC(MatrixMult00_READ_ADDR, buffer, 4);//64 bytes per row
            *(O_bitmap + (64*i) + (4*j) + (16*u) + 0) = buffer[0];
            *(O_bitmap + (64*i) + (4*j) + (16*u) + 1) = buffer[1];
            *(O_bitmap + (64*i) + (4*j) + (16*u) + 2) = buffer[2];
            *(O_bitmap + (64*i) + (4*j) + (16*u) + 3) = buffer[3];
          }
        }else{
          for(int u = 0; u < 4; u ++){
            read_data_from_ACC(MatrixMult00_READ_ADDR, buffer, 4);//64 bytes per row
            *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 0) = buffer[0];
            *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 1) = buffer[1];
            *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 2) = buffer[2];
            *(Tmp_bitmap + (256*i) + (16*j) + (64*u) + 3) = buffer[3];
            
          }
        }
      }
    }
  }

  write_mat("output.txt");
}
