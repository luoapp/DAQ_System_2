./daqcmdm \
multi \
init \
"output samplerate=1000.e3 vfs=5. channels=0 clock=internal" \
"input samplerate=1000.e3 channels=16 nrows=500000 vfs=5 mode=singleended clock=internal" \
trigger=external \
"readdma trigger" \

