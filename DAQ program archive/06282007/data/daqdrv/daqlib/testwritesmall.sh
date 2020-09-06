./daqcmd \
init \
"output samplerate=1000.e3 vfs=5. channels=4 clock=internal" \
"wave channel0=sine,f=50.e3,a=0.5,cycles=10." \
"wave channel1=sine,f=50.e3,a=1.0,cycles=10." \
"wave channel2=sine,f=50.e3,a=1.5,cycles=10." \
"wave channel3=sine,f=50.e3,a=2.0,cycles=10." \
"input samplerate=1000.e3 channels=8 nrows=128 vfs=5 mode=singleended clock=internal" \
trigger=software \
"writedma trigger"