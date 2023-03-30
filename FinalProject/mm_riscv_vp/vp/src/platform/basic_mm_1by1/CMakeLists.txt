file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-basic_mm_1by1
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-basic_mm_1by1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-mm_1by1
        main.cpp)

target_link_libraries(riscv-vp-mm_1by1 rv32 platform-basic_mm_1by1 platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
