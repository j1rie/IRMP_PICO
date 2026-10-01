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
# 1a Netbox can't send, conflicts withRC6(A)
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
# 3c Melinera, conflicts with RC6(A), 0 address bits
#for i in $(seq 0 0); do
for irdata in \
		"01 081f 003f" \
		"02 001f 003f" \
		"03 001f 003f" \
		"04 001f 003f" \
		"05 001f 003f" \
		"06 001f 003f" \
		"07 001f 003f" \
		"08 001f 003e" \
		"09 001f 003f" \
		"10 001f 003f" \
		"11 001f 003f" \
		"12 001f 003f" \
		"13 001f 003f" \
		"15 001f 003f" \
		"16 001f 003f" \
		"17 001f 003e" \
		"18 001f 003f" \
		"19 001f 003f" \
		"20 001f 003f" \
		"21 001f 003f" \
		"22 001f 003f" \
		"24 001f 003f" \
		"27 001f 003f" \
		"28 001f 003f" \
		"29 001f 003f" \
		"30 001f 003f" \
		"31 001f 003f" \
		"32 004f 003f" \
		"34 001f 003f" \
		"39 001f 003f" \
		"40 001f 003f" \
		"41 001f 003f" \
		"47 001f 003f" ;
do
	echo "${irdata}"
	./irsnd-25kHz ${irdata} | ./irmp-25kHz
done
#done
