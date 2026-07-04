rm ./generated/ws2812.pio.h
mkdir build_pico
cd build_pico
cmake -D"PICO_BOARD=pico" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_one
cd build_one
cmake -D"PICO_BOARD=waveshare_rp2040_one" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_zero
cd build_zero
cmake  -D"PICO_BOARD=waveshare_rp2040_zero" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_pico_2
cd build_pico_2
cmake -D"PICO_BOARD=pico2" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_seeed_xiao_rp2350
cd build_seeed_xiao_rp2350
cmake -D"PICO_BOARD=seeed_xiao_rp2350" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_zero_rp2350
cd build_zero_rp2350
cmake  -D"PICO_BOARD=waveshare_rp2350_pizero" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
mkdir build_weact_studio_rp2350a_core_v10_4mb
cd build_weact_studio_rp2350a_core_v10_4mb
cmake  -D"PICO_BOARD=weact_studio_rp2350a_core_v10_4mb" ..
make -j5
cd ..
rm ./generated/ws2812.pio.h
