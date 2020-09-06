#!/bin/sh
starttime="04:53pm May 15"
repeat=1500
period=2
for ((i=0; i<$repeat; i++))
do
 i1=`expr $i \* $period`
 #echo $i1
 echo "/data/udel_job.sh" | at $starttime+$i1'min'
done


