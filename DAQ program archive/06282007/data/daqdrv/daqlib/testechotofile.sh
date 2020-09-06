./daqcmd \
init \
"output samplerate=100.e3 vfs=5. channels=1 clock=internal" \
"wave channel0=sine,f=10.e3,a=0.5,cycles=200000." \
"input samplerate=100.e3 channels=8 nrows=10000 vfs=5 mode=singleended clock=internal" \
trigger=software \
"writereadtofile echotofile.data 2000000 trigger"

echo "converting to ascii"
./binarytoascii echotofile.data > echotofile.ascii
