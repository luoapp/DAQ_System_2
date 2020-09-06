./daqcmd \
init \
"output samplerate=1000.e3 vfs=5. channels=0 clock=internal" \
"input samplerate=1000.e3 channels=8 nrows=1000 vfs=5 mode=singleended clock=internal" \
trigger=software \
"readdma trigger binary readtobinarysmall.data"
