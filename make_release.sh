#! /bin/bash -e
tar -C RP2xxx/build -cJvf firmware_for_RP2xxx.tar.xz .

cd binaries_for_Windows
zip -9 -r ../binaries_for_Windows.zip *
cd ..

mkdir tmp
cp -p irmpalarm/Linux/irmpalarm irmpconfig/Linux/irmpconfig irmpconfig_gui/Linux/irmpconfig_gui irmpstatusled/irmpstatusled picoflash/picoflash irmpemit/irmpemit tmp/
tar -C tmp -cJvf binaries_for_Linux.tar.xz .
rm -r tmp
