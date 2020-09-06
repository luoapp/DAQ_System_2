./daqcmd \
"output samplerate=500.e3 vfs=5. channels=1 clock=internal" \
"wave channel0=sine,f=50.e3,a=5.,cycles=10." \
"input samplerate=500.e3 channels=8 nrows=128 vfs=5 mode=singleended clock=internal" \
trigger=software \
writepio trigger \
registers \
readpio
