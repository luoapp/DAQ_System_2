#echo "udel daq started ... "
#/data/beep/beep -r20 -d1000 -f800 -l100 &
#echo "/data/daqdrv64/samples/udel_readonly i.sr=80000 i.length=60 x.disp=0 x.hd=1" | at now+2min
#echo "reboot" | at now+122min
#/data/run_daqtest.sh &
#/data/daqdrv64/samples/udel_readonly i.sr=80000 i.length=60 x.disp=0 x.hd=1 >> /data/udel_daq.log
