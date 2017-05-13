#! /bin/bash

cd ../cacti_hp > /dev/null
VAL=$(./cacti -infile $1 | grep '  Area: ' | xargs python -c 'import sys; print(sys.argv[2])')
rm $1
cd ../tuner/ > /dev/null

if [[ -z $VAL ]]; then
    echo 'ERROR'
else
    echo $VAL
fi

exit 0
