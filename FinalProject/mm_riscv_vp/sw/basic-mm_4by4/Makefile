all : main.cpp
	riscv32-unknown-elf-g++ -std=c++14 main.cpp -o main -march=rv32ima -mabi=ilp32
	
sim: all
	~/ee6470/lab08/riscv-vp/vp/build/bin/riscv-vp-mm_4by4 --intercept-syscalls main
	
clean:
	rm -f main
