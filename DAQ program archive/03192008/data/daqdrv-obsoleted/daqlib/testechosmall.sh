./daqcmd \
init \
"output samplerate=1000.e3 vfs=5. channels=1 clock=internal" \
"wave channel0=sine,f=50.e3,a=0.1,cycles=50." \
"input samplerate=1000.e3 channels=2 nrows=2000 vfs=5 mode=singleended clock=internal" \
trigger=software \
"writereaddma trigger"

