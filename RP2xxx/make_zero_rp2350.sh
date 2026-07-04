rm ./generated/ws2812.pio.h
mkdir build_zero_rp2350
cd build_zero_rp2350
cmake  -D"PICO_BOARD=waveshare_rp2350_pizero" ..
make -j5
cd ..
