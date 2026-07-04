rm ./generated/ws2812.pio.h
mkdir build_weact_studio_rp2350a_core_v10_4mb
cd build_weact_studio_rp2350a_core_v10_4mb
cmake  -D"PICO_BOARD=weact_studio_rp2350a_core_v10_4mb" ..
make -j5
cd ..
