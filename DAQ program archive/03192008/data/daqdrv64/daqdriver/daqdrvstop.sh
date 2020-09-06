#!/bin/sh
module="daqdrv"
device="daqdrv"
mode="664"

/sbin/rmmod ${module}.ko

rm -f /dev/${device}[0-9]
