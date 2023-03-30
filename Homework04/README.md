# HW#04 Gaussian Filter on RISC-V  (VP)

#### How to build platform?

1. ```shell
   cd $EE6470
   ```

2. ```
   git clone https://github.com/enochliang/gaussian_riscv_vp.git
   ```

3. ```shell
   cp -r gaussian_riscv_vp/riscv-vp/vp/src/platform/* $EE6470/riscv-vp/vp/src/platform
   ```

4. ```shell
   cp -r gaussian_riscv_vp/riscv-vp/sw/* $EE6470/riscv-vp/sw
   ```

5. ```shell
   cd $EE6470/riscv-vp/vp/build
   ```

6. ```shell
   cmake ..
   ```

7. ```shell
   make install
   ```

How to build software?

1. ```shell
   cd $EE6470/riscv-vp/sw/basic-gaussian
   ```

2. ```
   make
   ```

How to run simulation?

```
make sim
```

The output image will be in present directory.









