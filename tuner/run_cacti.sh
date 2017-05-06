#! /bin/bash

cd ../cacti_hp > /dev/null
AREA=$(./cacti -infile $1 | grep ' Area: ' | awk 'BEGIN { FS = " Area: " } ; { print $2 }')
rm $1
cd ../tuner/ > /dev/null

if [[ -z $AREA ]]; then
    echo 'ERROR'
else
    echo $AREA
fi

exit 0
