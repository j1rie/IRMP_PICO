#!/bin/bash

for i in $(seq 0 0); do
#	../irmpemit/irmpemit -i 0x1e001f002${i}0a -d /dev/hidraw1
	../irmpemit/irmpemit -i 0x02001f002902 -d /dev/hidraw1
	sleep 0.135 # wait for release!
	../irmpemit/irmpemit -i 0x0b001f002f02 -d /dev/hidraw1
done
