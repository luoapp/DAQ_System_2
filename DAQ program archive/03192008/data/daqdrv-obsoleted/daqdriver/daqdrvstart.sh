#!/bin/sh
here=`pwd`
there=`dirname $0`

module="daqdrv"
device="daqdrv"
mode="666"

/sbin/insmod ${there}/${module}.ko

rm -f /dev/${device}[0-9]

major=`cat /proc/devices | grep daqdrv | awk '{ print \$1 }'`

echo $major

mknod /dev/${device}0 c ${major} 0
mknod /dev/${device}1 c ${major} 1
mknod /dev/${device}2 c ${major} 2
mknod /dev/${device}3 c ${major} 3

group=wheel

chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
