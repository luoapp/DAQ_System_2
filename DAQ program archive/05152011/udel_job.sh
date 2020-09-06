#! /bin/sh
# chirp1.daq : 45sec, 128 chirps
# mseq.daq : 8on-8off (x3)
logfile='/data/udel_daq.log'
recv_time=60
date -u >> $logfile
/data/daqdrv64/samples/udel_readwrite /data/transmission/chirp1amp10.daq o.sr=80000 x.hd=0 x.disp=0 o.nchans=4 i.length=10  >> $logfile
echo "------" >> $logfile
date -u >> $logfile
/data/daqdrv64/samples/udel_readonly i.sr=80000 i.length=$recv_time x.disp=0 x.hd=1 x.ch=0  >> $logfile
echo "-----sync hwc to sys" >> $logfile
hwclock --hctosys
echo "===========" >> $logfile
echo "" >> $logfile


