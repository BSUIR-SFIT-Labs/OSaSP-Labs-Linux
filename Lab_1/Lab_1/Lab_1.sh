#!/bin/bash

IFS=$'\n'
ERR="/tmp/errors.log"
exec 3>&2 2>$ERR

find "$2" -type f -name "*.$1" -exec ln {} "$3${A##*/}" \;

exec 2>&3 3>&-
sed "s/.[a-zA-Z]*:/`basename $0`:/" < $ERR 1>&2
rm $ERR
