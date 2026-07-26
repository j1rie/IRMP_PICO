#!/bin/bash

for i in $(seq 0 0); do
#	../irmpemit/irmpemit -i 0x03001f002${i}0a -d /dev/hidraw1
	../irmpemit/irmpemit -i 0x03001f003f01 -d /dev/hidraw1
#	sleep 0.135 # wait for release!
	../irmpemit/irmpemit -i 0x04001f003f01 -d /dev/hidraw1
done
