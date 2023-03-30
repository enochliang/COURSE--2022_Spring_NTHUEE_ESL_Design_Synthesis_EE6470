# Final Project : Multiple Matrix Multiplication PE Design Report

github repo: https://github.com/enochliang/gaussian_riscv_vp.git

## Introduction of the PE design

In this project, I modify the PE design from my midterm project. It has 4 different modes.

"Mode 0" is to input ***Weight*** matrix to the PE.

"Mode 1" is to input ***Input*** matrix and ***Bias*** matrix and then compute ***Output*** = ***Weight*** * ***Input*** + ***Bias*** and first output ***Input*** matrix then output ***Output*** matrix. 

"Mode 2" is to input ***Input*** matrix and then compute ***Output*** = ***Weight*** * ***Input*** and first output ***Input*** matrix then output ***Output*** matrix.

"Mode 3" is to input ***Input*** matrix and ***Bias*** matrix and then compute ***Output*** = ***Input*** + ***Bias*** and output ***Output*** matrix. 

And I connect them like TPU, the ***Input*** matrix will be send to the next PE on the right hand side, the ***Output*** matrix will be send to the next PE on the down side. Each data transfer is a 4*4 Matrix within 4 clock cycles.

## Introduction of the difference of PE number

In the project, I have multiple choices of number of PEs. I have (1 * 1), (2 * 2), (4 * 4) three kinds of configuration. And then compare their instruction number, total simulation cycle. And each configuration has different software because the platform architecture is different.

## Platform Source Code

I create three different module code to the platform folder. The first one is MM.cpp (Matrix Multiplication), this is the PE design to do the computation. The second one is RightBuf.cpp, this is to put between two PEs horizontally to transfer data. The third one is DownBuf.cpp, this is to put between two PEs vertically to transfer data.

And modify the main.cpp to change the module connection.

2*2 PEs for example.

The code below is module declaration.

```C++
MatrixMult MatrixMult00("MatrixMultM00");
MatrixMult MatrixMult01("MatrixMultM01");
MatrixMult MatrixMult10("MatrixMultM10");
MatrixMult MatrixMult11("MatrixMultM11");
DownBuf downbuf_00_to_10("downbuf_00_to_10");
DownBuf downbuf_01_to_11("downbuf_01_to_11");
RightBuf rightbuf_00_to_01("rightbuf_00_to_01");
RightBuf rightbuf_10_to_11("rightbuf_10_to_11");
```

And this is port mapping.

```C++
bus.ports[12] = new PortMapping(opt.MatrixMult00_start_addr, opt.MatrixMult00_end_addr);
bus.ports[13] = new PortMapping(opt.MatrixMult01_start_addr, opt.MatrixMult01_end_addr);
bus.ports[14] = new PortMapping(opt.MatrixMult10_start_addr, opt.MatrixMult10_end_addr);
bus.ports[15] = new PortMapping(opt.MatrixMult11_start_addr, opt.MatrixMult11_end_addr);
bus.ports[16] = new PortMapping(opt.downbuf_00_to_10_start_addr , opt.downbuf_00_to_10_end_addr);
bus.ports[17] = new PortMapping(opt.downbuf_01_to_11_start_addr , opt.downbuf_01_to_11_end_addr);
bus.ports[18] = new PortMapping(opt.rightbuf_00_to_01_start_addr, opt.rightbuf_00_to_01_end_addr);
bus.ports[19] = new PortMapping(opt.rightbuf_10_to_11_start_addr, opt.rightbuf_10_to_11_end_addr);
```

And this is binding.

```C++
bus.isocks[12].bind(MatrixMult00.tsock);
bus.isocks[13].bind(MatrixMult01.tsock);
bus.isocks[14].bind(MatrixMult10.tsock);
bus.isocks[15].bind(MatrixMult11.tsock);
bus.isocks[16].bind(downbuf_00_to_10.tsock);
bus.isocks[17].bind(downbuf_01_to_11.tsock);
bus.isocks[18].bind(rightbuf_00_to_01.tsock);
bus.isocks[19].bind(rightbuf_10_to_11.tsock);
```

## Software Source Code

Defining the address of accelerator.

```C++
static char* const MatrixMult00_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MatrixMult00_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);
static char* const MatrixMult01_START_ADDR = reinterpret_cast<char* const>(0x74000000);
static char* const MatrixMult01_READ_ADDR  = reinterpret_cast<char* const>(0x74000004);
static char* const MatrixMult10_START_ADDR = reinterpret_cast<char* const>(0x75000000);
static char* const MatrixMult10_READ_ADDR  = reinterpret_cast<char* const>(0x75000004);
static char* const MatrixMult11_START_ADDR = reinterpret_cast<char* const>(0x76000000);
static char* const MatrixMult11_READ_ADDR  = reinterpret_cast<char* const>(0x76000004);

static char* const DownBuf_00_to_10_START_ADDR = reinterpret_cast<char* const>(0x77000000);
static char* const DownBuf_00_to_10_READ_ADDR  = reinterpret_cast<char* const>(0x77000004);
static char* const DownBuf_01_to_11_START_ADDR = reinterpret_cast<char* const>(0x77001000);
static char* const DownBuf_01_to_11_READ_ADDR  = reinterpret_cast<char* const>(0x77001004);
static char* const RightBuf_00_to_01_START_ADDR = reinterpret_cast<char* const>(0x77002000);
static char* const RightBuf_00_to_01_READ_ADDR  = reinterpret_cast<char* const>(0x77002004);
static char* const RightBuf_10_to_11_START_ADDR = reinterpret_cast<char* const>(0x77003000);
static char* const RightBuf_10_to_11_READ_ADDR  = reinterpret_cast<char* const>(0x77003004);
```

Allocating the memory to store bitmaps.

W_bitmap is to store ***Weight*** matrix, I_bitmap is to store ***Input*** matrix, O_bitmap is to store ***Output*** matrix, Tmp_bitmap is to store the submatrices that need to be accumulated.

```C++
  char *W_bitmap;
  char *I_bitmap;
  char *O_bitmap;
  char *Tmp_bitmap;
  
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
      (char *)malloc((size_t)512);
  if (Tmp_bitmap == NULL) {
    printf("malloc Tmp error\n");
    return -1;
  }
```

And I define a new function, that is to transfer the datas between PEs and buffers, and I can choose whether to use DMA or memcpy().

```C++
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
```

## Simulation Result

According to these results, I find the number of instruction, the simulation cycles, and memory size will go down with the number of PEs increased.

|                       | **1\*1  PE** | **2\*2  PE** | **4\*4  PE** |
| --------------------- | ------------ | ------------ | ------------ |
| **Num_instr**         | 534563       | 495793       | 447870       |
| **Simulation_time**   | 18343220  ns | 16831340  ns | 15241360  ns |
| **Cycles**            | 1834322      | 1683134      | 1524136      |
| **BitmapStorageSize** | 1792  Bytes  | 1280  Bytes  | 768  Bytes   |

## PE Stratus HLS result

Area : 6040.3

