freq=$1
while [ "${freq}" == "" ];
do
  echo -n "Frequency ? "
  read freq
done

ampl=$2
while [ "${ampl}" == "" ];
do
  echo -n "Amplitude(volts) ? "
  read ampl
done

cycles=$3
while [ "${cycles}" == "" ];
do
  echo -n "Cycles(duration*f) ? "
  read cycles
done

filename=$4
while [ "${filename}" == "" ];
do
  echo -n "Filename ? "
  read filename
done

echo "$0 $freq $ampl $cycles $filename" > ${0}.repeat
chmod a+x ${0}.repeat

/data/daqdrv/daqlib/daqcmd \
init \
"output samplerate=1000.e3 vfs=10. channels=1 clock=internal" \
"wave channel0=sine,f=${freq},a=${ampl},cycles=${cycles} save ${filename}"
