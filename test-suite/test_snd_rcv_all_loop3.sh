#!/bin/bash

for i in $(seq 0 4); do
	../irmpemit/irmpemit -i 0x07001f002${i}00 -d /dev/hidraw1
done
