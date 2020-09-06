./daqcmdm \
multi \
init \
"output samplerate=1000.e3 vfs=5. channels=8 clock=internal" \
"wave channel0=sine,f=50.e3,a=0.5,cycles=10000." \
"wave channel1=sine,f=50.e3,a=1.0,cycles=10000." \
"wave channel2=sine,f=50.e3,a=1.5,cycles=10000." \
"wave channel3=sine,f=50.e3,a=2.0,cycles=10000." \
"wave channel4=sine,f=50.e3,a=2.5,cycles=10000." \
"wave channel5=sine,f=50.e3,a=3.0,cycles=10000." \
"wave channel6=sine,f=50.e3,a=3.5,cycles=10000." \
"wave channel7=sine,f=50.e3,a=4.5,cycles=10000." \
"input samplerate=1000.e3 channels=0 nrows=500000 vfs=5 mode=singleended clock=internal" \
trigger=software \
"writedma trigger" \

