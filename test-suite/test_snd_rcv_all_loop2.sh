#!/bin/bash

# 01 Sony
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
		01001f003f0f \
		02001f003f0f \
		03001f003f0f \
		04001f003f0f \
		05001f003f0f \
		06001f003f0f \
		07001f003f0f \
		08001f003e0f \
		09001f003f0f \
		0a001f003f0f \
		0b001f003f0f \
		0c001f003f0f \
		0d001f003f0f \
		0f001f003f0f \
		10001f003f0f \
		11001f003e0f \
		12001f003f0f \
		13001f003f0f \
		14001f003f0f \
		15001f003f0f \
		16001f003f0f \
		18001f003f0f \
		1b001f003f0f \
		1c001f003f0f \
		1d001f003f0f \
		1e001f003f0f \
		1f001f003f0f \
		20004f003f0f \
		22001f003f0f \
		27001f003f0f \
		28001f003f0f \
		29001f003f0f \
		2f001f003f0f \
		3c001f003f0f ;
do
	echo "${irdata}"
	../irmpemit/irmpemit -i 0x${irdata} -d /dev/hidraw1 > /dev/null 2>&1
done
#done
