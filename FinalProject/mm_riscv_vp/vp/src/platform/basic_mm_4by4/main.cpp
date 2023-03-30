#include <cstdlib>
#include <ctime>

#include "basic_timer.h"
#include "core/common/clint.h"
#include "display.hpp"
#include "dma.h"
#include "elf_loader.h"
#include "ethernet.h"
#include "fe310_plic.h"
#include "flash.h"
#include "debug_memory.h"
#include "iss.h"
#include "mem.h"
#include "memory.h"
#include "mram.h"
#include "sensor.h"
#include "sensor2.h"
#include "syscall.h"
#include "terminal.h"
#include "MM.h"
#include "DownBuf.h"
#include "RightBuf.h"
#include "util/options.h"
#include "platform/common/options.h"

#include "gdb-mc/gdb_server.h"
#include "gdb-mc/gdb_runner.h"

#include <boost/io/ios_state.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>

using namespace rv32;
namespace po = boost::program_options;

class BasicOptions : public Options {
public:
	typedef unsigned int addr_t;

	std::string mram_image;
	std::string flash_device;
	std::string network_device;
	std::string test_signature;

	addr_t mem_size = 1024 * 1024 * 32;  // 32 MB ram, to place it before the CLINT and run the base examples (assume
	                                     // memory start at zero) without modifications
	addr_t mem_start_addr = 0x00000000;
	addr_t mem_end_addr = mem_start_addr + mem_size - 1;
	addr_t clint_start_addr = 0x02000000;
	addr_t clint_end_addr = 0x0200ffff;
	addr_t sys_start_addr = 0x02010000;
	addr_t sys_end_addr = 0x020103ff;
	addr_t term_start_addr = 0x20000000;
	addr_t term_end_addr = term_start_addr + 16;
	addr_t ethernet_start_addr = 0x30000000;
	addr_t ethernet_end_addr = ethernet_start_addr + 1500;
	addr_t plic_start_addr = 0x40000000;
	addr_t plic_end_addr = 0x41000000;
	addr_t sensor_start_addr = 0x50000000;
	addr_t sensor_end_addr = 0x50001000;
	addr_t sensor2_start_addr = 0x50002000;
	addr_t sensor2_end_addr = 0x50004000;
	addr_t mram_start_addr = 0x60000000;
	addr_t mram_size = 0x10000000;
	addr_t mram_end_addr = mram_start_addr + mram_size - 1;
	addr_t dma_start_addr = 0x70000000;
	addr_t dma_end_addr = 0x70001000;
	addr_t flash_start_addr = 0x71000000;
	addr_t flash_end_addr = flash_start_addr + Flashcontroller::ADDR_SPACE;  // Usually 528 Byte
	addr_t display_start_addr = 0x72000000;
	addr_t display_end_addr = display_start_addr + Display::addressRange;

	addr_t MatrixMult00_start_addr = 0x73000000;
 	addr_t MatrixMult00_size = 0x01000000;
 	addr_t MatrixMult00_end_addr = MatrixMult00_start_addr + MatrixMult00_size - 1;
	addr_t MatrixMult01_start_addr = 0x74000000;
 	addr_t MatrixMult01_size = 0x01000000;
 	addr_t MatrixMult01_end_addr = MatrixMult01_start_addr + MatrixMult01_size - 1;
	addr_t MatrixMult02_start_addr = 0x75000000;
 	addr_t MatrixMult02_size = 0x01000000;
 	addr_t MatrixMult02_end_addr = MatrixMult02_start_addr + MatrixMult02_size - 1;
	addr_t MatrixMult03_start_addr = 0x76000000;
 	addr_t MatrixMult03_size = 0x01000000;
 	addr_t MatrixMult03_end_addr = MatrixMult03_start_addr + MatrixMult03_size - 1;

	addr_t MatrixMult10_start_addr = 0x77000000;
 	addr_t MatrixMult10_size = 0x01000000;
 	addr_t MatrixMult10_end_addr = MatrixMult10_start_addr + MatrixMult10_size - 1;
	addr_t MatrixMult11_start_addr = 0x78000000;
 	addr_t MatrixMult11_size = 0x01000000;
 	addr_t MatrixMult11_end_addr = MatrixMult11_start_addr + MatrixMult11_size - 1;
	addr_t MatrixMult12_start_addr = 0x79000000;
 	addr_t MatrixMult12_size = 0x01000000;
 	addr_t MatrixMult12_end_addr = MatrixMult12_start_addr + MatrixMult12_size - 1;
	addr_t MatrixMult13_start_addr = 0x7a000000;
 	addr_t MatrixMult13_size = 0x01000000;
 	addr_t MatrixMult13_end_addr = MatrixMult13_start_addr + MatrixMult13_size - 1;

	addr_t MatrixMult20_start_addr = 0x7b000000;
 	addr_t MatrixMult20_size = 0x01000000;
 	addr_t MatrixMult20_end_addr = MatrixMult20_start_addr + MatrixMult20_size - 1;
	addr_t MatrixMult21_start_addr = 0x7c000000;
 	addr_t MatrixMult21_size = 0x01000000;
 	addr_t MatrixMult21_end_addr = MatrixMult21_start_addr + MatrixMult21_size - 1;
	addr_t MatrixMult22_start_addr = 0x7d000000;
 	addr_t MatrixMult22_size = 0x01000000;
 	addr_t MatrixMult22_end_addr = MatrixMult22_start_addr + MatrixMult22_size - 1;
	addr_t MatrixMult23_start_addr = 0x7e000000;
 	addr_t MatrixMult23_size = 0x01000000;
 	addr_t MatrixMult23_end_addr = MatrixMult23_start_addr + MatrixMult23_size - 1;

	addr_t MatrixMult30_start_addr = 0x7f000000;
 	addr_t MatrixMult30_size = 0x01000000;
 	addr_t MatrixMult30_end_addr = MatrixMult30_start_addr + MatrixMult30_size - 1;
	addr_t MatrixMult31_start_addr = 0x80000000;
 	addr_t MatrixMult31_size = 0x01000000;
 	addr_t MatrixMult31_end_addr = MatrixMult31_start_addr + MatrixMult31_size - 1;
	addr_t MatrixMult32_start_addr = 0x81000000;
 	addr_t MatrixMult32_size = 0x01000000;
 	addr_t MatrixMult32_end_addr = MatrixMult32_start_addr + MatrixMult32_size - 1;
	addr_t MatrixMult33_start_addr = 0x82000000;
 	addr_t MatrixMult33_size = 0x01000000;
 	addr_t MatrixMult33_end_addr = MatrixMult33_start_addr + MatrixMult33_size - 1;
	
	addr_t downbuf_00_to_10_start_addr = 0x83000000;
	addr_t downbuf_10_to_20_start_addr = 0x83001000;
	addr_t downbuf_20_to_30_start_addr = 0x83002000;
	addr_t downbuf_01_to_11_start_addr = 0x83003000;
	addr_t downbuf_11_to_21_start_addr = 0x83004000;
	addr_t downbuf_21_to_31_start_addr = 0x83005000;
	addr_t downbuf_02_to_12_start_addr = 0x83006000;
	addr_t downbuf_12_to_22_start_addr = 0x83007000;
	addr_t downbuf_22_to_32_start_addr = 0x83008000;
	addr_t downbuf_03_to_13_start_addr = 0x83009000;
	addr_t downbuf_13_to_23_start_addr = 0x8300a000;
	addr_t downbuf_23_to_33_start_addr = 0x8300b000;
	addr_t downbuf_00_to_10_size = 0x00001000;
	addr_t downbuf_10_to_20_size = 0x00001000;
	addr_t downbuf_20_to_30_size = 0x00001000;
	addr_t downbuf_01_to_11_size = 0x00001000;
	addr_t downbuf_11_to_21_size = 0x00001000;
	addr_t downbuf_21_to_31_size = 0x00001000;
	addr_t downbuf_02_to_12_size = 0x00001000;
	addr_t downbuf_12_to_22_size = 0x00001000;
	addr_t downbuf_22_to_32_size = 0x00001000;
	addr_t downbuf_03_to_13_size = 0x00001000;
	addr_t downbuf_13_to_23_size = 0x00001000;
	addr_t downbuf_23_to_33_size = 0x00001000;
	addr_t downbuf_00_to_10_end_addr = downbuf_00_to_10_start_addr + downbuf_00_to_10_size - 1;
	addr_t downbuf_10_to_20_end_addr = downbuf_10_to_20_start_addr + downbuf_10_to_20_size - 1;
	addr_t downbuf_20_to_30_end_addr = downbuf_20_to_30_start_addr + downbuf_20_to_30_size - 1;
	addr_t downbuf_01_to_11_end_addr = downbuf_01_to_11_start_addr + downbuf_01_to_11_size - 1;
	addr_t downbuf_11_to_21_end_addr = downbuf_11_to_21_start_addr + downbuf_11_to_21_size - 1;
	addr_t downbuf_21_to_31_end_addr = downbuf_21_to_31_start_addr + downbuf_21_to_31_size - 1;
	addr_t downbuf_02_to_12_end_addr = downbuf_02_to_12_start_addr + downbuf_02_to_12_size - 1;
	addr_t downbuf_12_to_22_end_addr = downbuf_12_to_22_start_addr + downbuf_12_to_22_size - 1;
	addr_t downbuf_22_to_32_end_addr = downbuf_22_to_32_start_addr + downbuf_22_to_32_size - 1;
	addr_t downbuf_03_to_13_end_addr = downbuf_03_to_13_start_addr + downbuf_03_to_13_size - 1;
	addr_t downbuf_13_to_23_end_addr = downbuf_13_to_23_start_addr + downbuf_13_to_23_size - 1;
	addr_t downbuf_23_to_33_end_addr = downbuf_23_to_33_start_addr + downbuf_23_to_33_size - 1;
	
	addr_t rightbuf_00_to_01_start_addr = 0x8300c000;
	addr_t rightbuf_01_to_02_start_addr = 0x8300d000;
	addr_t rightbuf_02_to_03_start_addr = 0x8300e000;
	addr_t rightbuf_10_to_11_start_addr = 0x8300f000;
	addr_t rightbuf_11_to_12_start_addr = 0x83010000;
	addr_t rightbuf_12_to_13_start_addr = 0x83011000;
	addr_t rightbuf_20_to_21_start_addr = 0x83012000;
	addr_t rightbuf_21_to_22_start_addr = 0x83013000;
	addr_t rightbuf_22_to_23_start_addr = 0x83014000;
	addr_t rightbuf_30_to_31_start_addr = 0x83015000;
	addr_t rightbuf_31_to_32_start_addr = 0x83016000;
	addr_t rightbuf_32_to_33_start_addr = 0x83017000;
	addr_t rightbuf_00_to_01_size = 0x00001000;
	addr_t rightbuf_01_to_02_size = 0x00001000;
	addr_t rightbuf_02_to_03_size = 0x00001000;
	addr_t rightbuf_10_to_11_size = 0x00001000;
	addr_t rightbuf_11_to_12_size = 0x00001000;
	addr_t rightbuf_12_to_13_size = 0x00001000;
	addr_t rightbuf_20_to_21_size = 0x00001000;
	addr_t rightbuf_21_to_22_size = 0x00001000;
	addr_t rightbuf_22_to_23_size = 0x00001000;
	addr_t rightbuf_30_to_31_size = 0x00001000;
	addr_t rightbuf_31_to_32_size = 0x00001000;
	addr_t rightbuf_32_to_33_size = 0x00001000;
	addr_t rightbuf_00_to_01_end_addr = rightbuf_00_to_01_start_addr + rightbuf_00_to_01_size - 1;
	addr_t rightbuf_01_to_02_end_addr = rightbuf_01_to_02_start_addr + rightbuf_01_to_02_size - 1;
	addr_t rightbuf_02_to_03_end_addr = rightbuf_02_to_03_start_addr + rightbuf_02_to_03_size - 1;
	addr_t rightbuf_10_to_11_end_addr = rightbuf_10_to_11_start_addr + rightbuf_10_to_11_size - 1;
	addr_t rightbuf_11_to_12_end_addr = rightbuf_11_to_12_start_addr + rightbuf_11_to_12_size - 1;
	addr_t rightbuf_12_to_13_end_addr = rightbuf_12_to_13_start_addr + rightbuf_12_to_13_size - 1;
	addr_t rightbuf_20_to_21_end_addr = rightbuf_20_to_21_start_addr + rightbuf_20_to_21_size - 1;
	addr_t rightbuf_21_to_22_end_addr = rightbuf_21_to_22_start_addr + rightbuf_21_to_22_size - 1;
	addr_t rightbuf_22_to_23_end_addr = rightbuf_22_to_23_start_addr + rightbuf_22_to_23_size - 1;
	addr_t rightbuf_30_to_31_end_addr = rightbuf_30_to_31_start_addr + rightbuf_30_to_31_size - 1;
	addr_t rightbuf_31_to_32_end_addr = rightbuf_31_to_32_start_addr + rightbuf_31_to_32_size - 1;
	addr_t rightbuf_32_to_33_end_addr = rightbuf_32_to_33_start_addr + rightbuf_32_to_33_size - 1;

	bool use_E_base_isa = false;

	OptionValue<unsigned long> entry_point;

	BasicOptions(void) {
        	// clang-format off
		add_options()
			("memory-start", po::value<unsigned int>(&mem_start_addr),"set memory start address")
			("memory-size", po::value<unsigned int>(&mem_size), "set memory size")
			("use-E-base-isa", po::bool_switch(&use_E_base_isa), "use the E instead of the I integer base ISA")
			("entry-point", po::value<std::string>(&entry_point.option),"set entry point address (ISS program counter)")
			("mram-image", po::value<std::string>(&mram_image)->default_value(""),"MRAM image file for persistency")
			("mram-image-size", po::value<unsigned int>(&mram_size), "MRAM image size")
			("flash-device", po::value<std::string>(&flash_device)->default_value(""),"blockdevice for flash emulation")
			("network-device", po::value<std::string>(&network_device)->default_value(""),"name of the tap network adapter, e.g. /dev/tap6")
			("signature", po::value<std::string>(&test_signature)->default_value(""),"output filename for the test execution signature");
        	// clang-format on
	}

	void parse(int argc, char **argv) override {
		Options::parse(argc, argv);

		entry_point.finalize(parse_ulong_option);
		mem_end_addr = mem_start_addr + mem_size - 1;
		assert((mem_end_addr < clint_start_addr || mem_start_addr > rightbuf_10_to_11_end_addr) &&
		       "RAM too big, would overlap memory");
		mram_end_addr = mram_start_addr + mram_size - 1;
		assert(mram_end_addr < dma_start_addr && "MRAM too big, would overlap memory");
	}
};

std::ostream& operator<<(std::ostream& os, const BasicOptions& o) {
	os << std::hex;
	os << "mem_start_addr:\t" << +o.mem_start_addr << std::endl;
	os << "mem_end_addr:\t"   << +o.mem_end_addr   << std::endl;
	os << static_cast <const Options&>( o );
	return os;
}

int sc_main(int argc, char **argv) {
	BasicOptions opt;
	opt.parse(argc, argv);

	std::srand(std::time(nullptr));  // use current time as seed for random generator

	tlm::tlm_global_quantum::instance().set(sc_core::sc_time(opt.tlm_global_quantum, sc_core::SC_NS));

	ISS core(0, opt.use_E_base_isa);
	SimpleMemory mem("SimpleMemory", opt.mem_size);
	SimpleTerminal term("SimpleTerminal");
	ELFLoader loader(opt.input_program.c_str());
	SimpleBus<3, 52> bus("SimpleBus");
	CombinedMemoryInterface iss_mem_if("MemoryInterface", core);
	SyscallHandler sys("SyscallHandler");
	FE310_PLIC<1, 64, 96, 32> plic("PLIC");
	CLINT<1> clint("CLINT");
	SimpleSensor sensor("SimpleSensor", 2);
	SimpleSensor2 sensor2("SimpleSensor2", 5);
	BasicTimer timer("BasicTimer", 3);
	SimpleMRAM mram("SimpleMRAM", opt.mram_image, opt.mram_size);
	SimpleDMA dma("SimpleDMA", 4);
	Flashcontroller flashController("Flashcontroller", opt.flash_device);
	EthernetDevice ethernet("EthernetDevice", 7, mem.data, opt.network_device);
	Display display("Display");
	DebugMemoryInterface dbg_if("DebugMemoryInterface");
	MatrixMult MatrixMult00("MatrixMult00");
	MatrixMult MatrixMult01("MatrixMult01");
	MatrixMult MatrixMult02("MatrixMult02");
	MatrixMult MatrixMult03("MatrixMult03");
	MatrixMult MatrixMult10("MatrixMult10");
	MatrixMult MatrixMult11("MatrixMult11");
	MatrixMult MatrixMult12("MatrixMult12");
	MatrixMult MatrixMult13("MatrixMult13");
	MatrixMult MatrixMult20("MatrixMult20");
	MatrixMult MatrixMult21("MatrixMult21");
	MatrixMult MatrixMult22("MatrixMult22");
	MatrixMult MatrixMult23("MatrixMult23");
	MatrixMult MatrixMult30("MatrixMult30");
	MatrixMult MatrixMult31("MatrixMult31");
	MatrixMult MatrixMult32("MatrixMult32");
	MatrixMult MatrixMult33("MatrixMult33");

	DownBuf downbuf_00_to_10("downbuf_00_to_10");
	DownBuf downbuf_10_to_20("downbuf_10_to_20");
	DownBuf downbuf_20_to_30("downbuf_20_to_30");
	DownBuf downbuf_01_to_11("downbuf_01_to_11");
	DownBuf downbuf_11_to_21("downbuf_11_to_21");
	DownBuf downbuf_21_to_31("downbuf_21_to_31");
	DownBuf downbuf_02_to_12("downbuf_02_to_12");
	DownBuf downbuf_12_to_22("downbuf_12_to_22");
	DownBuf downbuf_22_to_32("downbuf_22_to_32");
	DownBuf downbuf_03_to_13("downbuf_03_to_13");
	DownBuf downbuf_13_to_23("downbuf_13_to_23");
	DownBuf downbuf_23_to_33("downbuf_23_to_33");
	RightBuf rightbuf_00_to_01("rightbuf_00_to_01");
	RightBuf rightbuf_01_to_02("rightbuf_01_to_02");
	RightBuf rightbuf_02_to_03("rightbuf_02_to_03");
	RightBuf rightbuf_10_to_11("rightbuf_10_to_11");
	RightBuf rightbuf_11_to_12("rightbuf_11_to_12");
	RightBuf rightbuf_12_to_13("rightbuf_12_to_13");
	RightBuf rightbuf_20_to_21("rightbuf_20_to_21");
	RightBuf rightbuf_21_to_22("rightbuf_21_to_22");
	RightBuf rightbuf_22_to_23("rightbuf_22_to_23");
	RightBuf rightbuf_30_to_31("rightbuf_30_to_31");
	RightBuf rightbuf_31_to_32("rightbuf_31_to_32");
	RightBuf rightbuf_32_to_33("rightbuf_32_to_33");

	MemoryDMI dmi = MemoryDMI::create_start_size_mapping(mem.data, opt.mem_start_addr, mem.size);
	InstrMemoryProxy instr_mem(dmi, core);

	std::shared_ptr<BusLock> bus_lock = std::make_shared<BusLock>();
	iss_mem_if.bus_lock = bus_lock;

	instr_memory_if *instr_mem_if = &iss_mem_if;
	data_memory_if *data_mem_if = &iss_mem_if;
	if (opt.use_instr_dmi)
		instr_mem_if = &instr_mem;
	if (opt.use_data_dmi) {
		iss_mem_if.dmi_ranges.emplace_back(dmi);
	}

	uint64_t entry_point = loader.get_entrypoint();
	if (opt.entry_point.available)
		entry_point = opt.entry_point.value;
	try {
		loader.load_executable_image(mem, mem.size, opt.mem_start_addr);
	} catch(ELFLoader::load_executable_exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Memory map: " << std::endl;
		std::cerr << opt << std::endl;
		return -1;
	}
	core.init(instr_mem_if, data_mem_if, &clint, entry_point, rv32_align_address(opt.mem_end_addr));
	sys.init(mem.data, opt.mem_start_addr, loader.get_heap_addr());
	sys.register_core(&core);

	if (opt.intercept_syscalls)
		core.sys = &sys;

	// address mapping
	bus.ports[0] = new PortMapping(opt.mem_start_addr, opt.mem_end_addr);
	bus.ports[1] = new PortMapping(opt.clint_start_addr, opt.clint_end_addr);
	bus.ports[2] = new PortMapping(opt.plic_start_addr, opt.plic_end_addr);
	bus.ports[3] = new PortMapping(opt.term_start_addr, opt.term_end_addr);
	bus.ports[4] = new PortMapping(opt.sensor_start_addr, opt.sensor_end_addr);
	bus.ports[5] = new PortMapping(opt.dma_start_addr, opt.dma_end_addr);
	bus.ports[6] = new PortMapping(opt.sensor2_start_addr, opt.sensor2_end_addr);
	bus.ports[7] = new PortMapping(opt.mram_start_addr, opt.mram_end_addr);
	bus.ports[8] = new PortMapping(opt.flash_start_addr, opt.flash_end_addr);
	bus.ports[9] = new PortMapping(opt.ethernet_start_addr, opt.ethernet_end_addr);
	bus.ports[10] = new PortMapping(opt.display_start_addr, opt.display_end_addr);
	bus.ports[11] = new PortMapping(opt.sys_start_addr, opt.sys_end_addr);
	bus.ports[12] = new PortMapping(opt.MatrixMult00_start_addr, opt.MatrixMult00_end_addr);
	bus.ports[13] = new PortMapping(opt.MatrixMult01_start_addr, opt.MatrixMult01_end_addr);
	bus.ports[14] = new PortMapping(opt.MatrixMult02_start_addr, opt.MatrixMult02_end_addr);
	bus.ports[15] = new PortMapping(opt.MatrixMult03_start_addr, opt.MatrixMult03_end_addr);
	bus.ports[16] = new PortMapping(opt.MatrixMult10_start_addr, opt.MatrixMult10_end_addr);
	bus.ports[17] = new PortMapping(opt.MatrixMult11_start_addr, opt.MatrixMult11_end_addr);
	bus.ports[18] = new PortMapping(opt.MatrixMult12_start_addr, opt.MatrixMult12_end_addr);
	bus.ports[19] = new PortMapping(opt.MatrixMult13_start_addr, opt.MatrixMult13_end_addr);
	bus.ports[20] = new PortMapping(opt.MatrixMult20_start_addr, opt.MatrixMult20_end_addr);
	bus.ports[21] = new PortMapping(opt.MatrixMult21_start_addr, opt.MatrixMult21_end_addr);
	bus.ports[22] = new PortMapping(opt.MatrixMult22_start_addr, opt.MatrixMult22_end_addr);
	bus.ports[23] = new PortMapping(opt.MatrixMult23_start_addr, opt.MatrixMult23_end_addr);
	bus.ports[24] = new PortMapping(opt.MatrixMult30_start_addr, opt.MatrixMult30_end_addr);
	bus.ports[25] = new PortMapping(opt.MatrixMult31_start_addr, opt.MatrixMult31_end_addr);
	bus.ports[26] = new PortMapping(opt.MatrixMult32_start_addr, opt.MatrixMult32_end_addr);
	bus.ports[27] = new PortMapping(opt.MatrixMult33_start_addr, opt.MatrixMult33_end_addr);
	bus.ports[28] = new PortMapping(opt.downbuf_00_to_10_start_addr , opt.downbuf_00_to_10_end_addr);
	bus.ports[29] = new PortMapping(opt.downbuf_10_to_20_start_addr , opt.downbuf_10_to_20_end_addr);
	bus.ports[30] = new PortMapping(opt.downbuf_20_to_30_start_addr , opt.downbuf_20_to_30_end_addr);
	bus.ports[31] = new PortMapping(opt.downbuf_01_to_11_start_addr , opt.downbuf_01_to_11_end_addr);
	bus.ports[32] = new PortMapping(opt.downbuf_11_to_21_start_addr , opt.downbuf_11_to_21_end_addr);
	bus.ports[33] = new PortMapping(opt.downbuf_21_to_31_start_addr , opt.downbuf_21_to_31_end_addr);
	bus.ports[34] = new PortMapping(opt.downbuf_02_to_12_start_addr , opt.downbuf_02_to_12_end_addr);
	bus.ports[35] = new PortMapping(opt.downbuf_12_to_22_start_addr , opt.downbuf_12_to_22_end_addr);
	bus.ports[36] = new PortMapping(opt.downbuf_22_to_32_start_addr , opt.downbuf_22_to_32_end_addr);
	bus.ports[37] = new PortMapping(opt.downbuf_03_to_13_start_addr , opt.downbuf_03_to_13_end_addr);
	bus.ports[38] = new PortMapping(opt.downbuf_13_to_23_start_addr , opt.downbuf_13_to_23_end_addr);
	bus.ports[39] = new PortMapping(opt.downbuf_23_to_33_start_addr , opt.downbuf_23_to_33_end_addr);
	bus.ports[40] = new PortMapping(opt.rightbuf_00_to_01_start_addr, opt.rightbuf_00_to_01_end_addr);
	bus.ports[41] = new PortMapping(opt.rightbuf_01_to_02_start_addr, opt.rightbuf_01_to_02_end_addr);
	bus.ports[42] = new PortMapping(opt.rightbuf_02_to_03_start_addr, opt.rightbuf_02_to_03_end_addr);
	bus.ports[43] = new PortMapping(opt.rightbuf_10_to_11_start_addr, opt.rightbuf_10_to_11_end_addr);
	bus.ports[44] = new PortMapping(opt.rightbuf_11_to_12_start_addr, opt.rightbuf_11_to_12_end_addr);
	bus.ports[45] = new PortMapping(opt.rightbuf_12_to_13_start_addr, opt.rightbuf_12_to_13_end_addr);
	bus.ports[46] = new PortMapping(opt.rightbuf_20_to_21_start_addr, opt.rightbuf_20_to_21_end_addr);
	bus.ports[47] = new PortMapping(opt.rightbuf_21_to_22_start_addr, opt.rightbuf_21_to_22_end_addr);
	bus.ports[48] = new PortMapping(opt.rightbuf_22_to_23_start_addr, opt.rightbuf_22_to_23_end_addr);
	bus.ports[49] = new PortMapping(opt.rightbuf_30_to_31_start_addr, opt.rightbuf_30_to_31_end_addr);
	bus.ports[50] = new PortMapping(opt.rightbuf_31_to_32_start_addr, opt.rightbuf_31_to_32_end_addr);
	bus.ports[51] = new PortMapping(opt.rightbuf_32_to_33_start_addr, opt.rightbuf_32_to_33_end_addr);

	// connect TLM sockets
	iss_mem_if.isock.bind(bus.tsocks[0]);
	dbg_if.isock.bind(bus.tsocks[2]);

	PeripheralWriteConnector dma_connector("SimpleDMA-Connector");  // to respect ISS bus locking
	dma_connector.isock.bind(bus.tsocks[1]);
	dma.isock.bind(dma_connector.tsock);
	dma_connector.bus_lock = bus_lock;

	bus.isocks[0].bind(mem.tsock);
	bus.isocks[1].bind(clint.tsock);
	bus.isocks[2].bind(plic.tsock);
	bus.isocks[3].bind(term.tsock);
	bus.isocks[4].bind(sensor.tsock);
	bus.isocks[5].bind(dma.tsock);
	bus.isocks[6].bind(sensor2.tsock);
	bus.isocks[7].bind(mram.tsock);
	bus.isocks[8].bind(flashController.tsock);
	bus.isocks[9].bind(ethernet.tsock);
	bus.isocks[10].bind(display.tsock);
	bus.isocks[11].bind(sys.tsock);
	bus.isocks[12].bind(MatrixMult00.tsock);
	bus.isocks[13].bind(MatrixMult01.tsock);
	bus.isocks[14].bind(MatrixMult02.tsock);
	bus.isocks[15].bind(MatrixMult03.tsock);
	bus.isocks[16].bind(MatrixMult10.tsock);
	bus.isocks[17].bind(MatrixMult11.tsock);
	bus.isocks[18].bind(MatrixMult12.tsock);
	bus.isocks[19].bind(MatrixMult13.tsock);
	bus.isocks[20].bind(MatrixMult20.tsock);
	bus.isocks[21].bind(MatrixMult21.tsock);
	bus.isocks[22].bind(MatrixMult22.tsock);
	bus.isocks[23].bind(MatrixMult23.tsock);
	bus.isocks[24].bind(MatrixMult30.tsock);
	bus.isocks[25].bind(MatrixMult31.tsock);
	bus.isocks[26].bind(MatrixMult32.tsock);
	bus.isocks[27].bind(MatrixMult33.tsock);
	bus.isocks[28].bind(downbuf_00_to_10.tsock);
	bus.isocks[29].bind(downbuf_10_to_20.tsock);
	bus.isocks[30].bind(downbuf_20_to_30.tsock);
	bus.isocks[31].bind(downbuf_01_to_11.tsock);
	bus.isocks[32].bind(downbuf_11_to_21.tsock);
	bus.isocks[33].bind(downbuf_21_to_31.tsock);
	bus.isocks[34].bind(downbuf_02_to_12.tsock);
	bus.isocks[35].bind(downbuf_12_to_22.tsock);
	bus.isocks[36].bind(downbuf_22_to_32.tsock);
	bus.isocks[37].bind(downbuf_03_to_13.tsock);
	bus.isocks[38].bind(downbuf_13_to_23.tsock);
	bus.isocks[39].bind(downbuf_23_to_33.tsock);
	bus.isocks[40].bind(rightbuf_00_to_01.tsock);
	bus.isocks[41].bind(rightbuf_01_to_02.tsock);
	bus.isocks[42].bind(rightbuf_02_to_03.tsock);
	bus.isocks[43].bind(rightbuf_10_to_11.tsock);
	bus.isocks[44].bind(rightbuf_11_to_12.tsock);
	bus.isocks[45].bind(rightbuf_12_to_13.tsock);
	bus.isocks[46].bind(rightbuf_20_to_21.tsock);
	bus.isocks[47].bind(rightbuf_21_to_22.tsock);
	bus.isocks[48].bind(rightbuf_22_to_23.tsock);
	bus.isocks[49].bind(rightbuf_30_to_31.tsock);
	bus.isocks[50].bind(rightbuf_31_to_32.tsock);
	bus.isocks[51].bind(rightbuf_32_to_33.tsock);



	// connect interrupt signals/communication
	plic.target_harts[0] = &core;
	clint.target_harts[0] = &core;
	sensor.plic = &plic;
	dma.plic = &plic;
	timer.plic = &plic;
	sensor2.plic = &plic;
	ethernet.plic = &plic;

	std::vector<debug_target_if *> threads;
	threads.push_back(&core);

	core.trace = opt.trace_mode;  // switch for printing instructions
	if (opt.use_debug_runner) {
		auto server = new GDBServer("GDBServer", threads, &dbg_if, opt.debug_port);
		new GDBServerRunner("GDBRunner", server, &core);
	} else {
		new DirectCoreRunner(core);
	}

	sc_core::sc_start();

	core.show();

	if (opt.test_signature != "") {
		auto begin_sig = loader.get_begin_signature_address();
		auto end_sig = loader.get_end_signature_address();

		{
			boost::io::ios_flags_saver ifs(cout);
			std::cout << std::hex;
			std::cout << "begin_signature: " << begin_sig << std::endl;
			std::cout << "end_signature: " << end_sig << std::endl;
			std::cout << "signature output file: " << opt.test_signature << std::endl;
		}

		assert(end_sig >= begin_sig);
		assert(begin_sig >= opt.mem_start_addr);

		auto begin = begin_sig - opt.mem_start_addr;
		auto end = end_sig - opt.mem_start_addr;

		ofstream sigfile(opt.test_signature, ios::out);

		auto n = begin;
		while (n < end) {
			sigfile << std::hex << std::setw(2) << std::setfill('0') << (unsigned)mem.data[n];
			++n;
		}
	}

	return 0;
}
