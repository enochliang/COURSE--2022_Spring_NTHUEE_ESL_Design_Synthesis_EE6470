file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-basic_mm_4by4
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-basic_mm_4by4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-mm_4by4
        main.cpp)

target_link_libraries(riscv-vp-mm_4by4 rv32 platform-basic_mm_4by4 platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
