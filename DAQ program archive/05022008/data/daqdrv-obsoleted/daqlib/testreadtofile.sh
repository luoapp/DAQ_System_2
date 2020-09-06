./daqcmd \
init \
"output samplerate=1000.e3 vfs=5. channels=4 clock=internal" \
"input samplerate=1.e6 channels=8 nrows=10000 vfs=5 mode=singleended clock=internal" \
trigger=software \
"readtofile readtofile.data 20000000 trigger"

echo "converting to ascii"
./binarytoascii readtofile.data > readtofile.ascii
