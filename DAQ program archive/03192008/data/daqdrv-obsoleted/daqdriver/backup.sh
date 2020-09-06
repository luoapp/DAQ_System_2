tgzfile="`pwd`.`date +%y%m%d.%H%M`.tgz"

tar -cvzf $tgzfile *

mv $tgzfile ../Archive/ -v

echo $tgzfile
