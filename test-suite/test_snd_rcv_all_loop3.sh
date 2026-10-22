#!/bin/bash

# 01 Sony, upper address byte 3 or 8 for sending address byte (special: 3 protocols in 1)
# 06 Recs80 3 address bits
# 08 Denon/Sharp only even commands, last 2 command bits 00 = Denon, 10 = Sharp
# 0c Recs80ext 4 address bits
# 0d Nubert 0 address bits
# 0e B&O 455kHz, 0 address bits
# 0f Grundig 0 address bits
# 11 Siemens last bit inverted to last but one
# 12 FDC special
# 13 RCCAR 2 address bits
# 14 JVC 4 address bits
# 16 Nikon 0 address bits, 2 data bits
# 17 Ruwido conflicts with denon, 56kHz
# 18 IR-60, 0 address bits
# 19 Kathrein can't send
# 1a Netbox can't send
# 1d Lego 0 address bits
# 1e Thomson 4 address bits
# 1f Bose 0 address bits
# 20 A1TVBox special
# 21 Ortek can't send
# 22 Telefunken 0 address bits
# 23 Roomba conflicts with RC6
# 24 RCMM32 can't send
# 25 RCMM24 can't send
# 26 RCMM12 can't send
# 27 Speaker 0 address bits
# 29 Samsung48 double
# 2a Merlin
# 2b Pentax max 16kHz
# 2c Fan conflicts with Nubert
# 2d S100 conflicts with RC5, can't send
# 2e ACP24 conflicts with Denon
# 2f Technics
# 30 Panasonic conflicts with Kaseikyo and Mitsu-Heavy
# 31 Mitsu-Heavy conflicts with Kaseikyo and Panasonic
# 3c Melinera 0 address bits
#for i in $(seq 0 199); do
for irdata in \
		81001f003f00 \
		82001f003f00 \
		83001f003f00 \
		84001f003f00 \
		85001f003f00 \
		87001f003f00 \
		88001f003e00 \
		89001f003f00 \
		8a001f003f00 \
		8b001f003f00 \
		8d001f003f00 \
		8f001f003f00 \
		90001f003f00 \
		92001f003f00 \
		93001f003f00 \
		94001f003f00 \
		95001f003f00 \
		96001f003f00 \
		98001f003f00 \
		9b001f003f00 \
		9c001f003f00 \
		9e001f003f00 \
		9f001f003f00 \
		a2001f003f00 \
		a7001f003f00 \
		a8001f003f00 \
		a9001f003f00 \
		af001f003f00 \
		bc001f003f00 \
		1d001f003f00;
do
	echo "${irdata}"
	../irmpemit/irmpemit -i 0x${irdata} -d /dev/hidraw1 > /dev/null 2>&1
	sleep 0.15
done
#done
