#----------------------------------------------------------------------------
# Makefile for Linux
#
# usage:
#
#        make -f makefile.lnx [all | [all-irmp | all-irsnd]
#
# Copyright (c) 2010-2012 Frank Meyer - frank(at)fli4l.de
#
# $Id: makefile.lnx,v 1.6 2010/06/11 14:47:24 fm Exp $
#----------------------------------------------------------------------------
all:	all-irmp all-irsnd

all-irmp:   irmp-10kHz irmp-15kHz irmp-20kHz irmp-25kHz irmp-50kHz

all-irsnd:  irsnd-10kHz irsnd-15kHz irsnd-20kHz irsnd-25kHz irsnd-50kHz

irmp-10kHz: irmp.c irmp.h irmpconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=10000 irmp.c -o irmp-10kHz

irmp-15kHz: irmp.c irmp.h irmpconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=15000 irmp.c -o irmp-15kHz

irmp-20kHz: irmp.c irmp.h irmpconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=20000 irmp.c -o irmp-20kHz

irmp-25kHz: irmp.c irmp.h irmpconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=25000 irmp.c -o irmp-25kHz

irmp-50kHz: irmp.c irmp.h irmpconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=50000 irmp.c -o irmp-50kHz

irsnd-10kHz: irsnd.c irsnd.h irmp.h irsndconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=10000 irsnd.c -o irsnd-10kHz

irsnd-15kHz: irsnd.c irsnd.h irmp.h irsndconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=15000 irsnd.c -o irsnd-15kHz

irsnd-20kHz: irsnd.c irsnd.h irmp.h irsndconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=20000 irsnd.c -o irsnd-20kHz

irsnd-25kHz: irsnd.c irsnd.h irmp.h irsndconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=25000 irsnd.c -o irsnd-25kHz

irsnd-50kHz: irsnd.c irsnd.h irmp.h irsndconfig.h irmpsystem.h irmpprotocols.h
	cc -Wall -DF_INTERRUPTS=50000 irsnd.c -o irsnd-50kHz


test: all
	cd IR-Data && ./test-suite.sh

clean:
	rm -f irmp-10kHz irmp-15kHz irmp-20kHz irmp-25kHz irmp-50kHz irsnd-10kHz irsnd-15kHz irsnd-20kHz irsnd-25kHz irsnd-50kHz
