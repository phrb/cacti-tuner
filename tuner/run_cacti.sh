#! /bin/bash

cd ../cacti_hp > /dev/null
AREA=$(./cacti -infile $1 | grep '  Area: ' | xargs python -c 'import sys; print(sys.argv[2])')
ACCT=$(./cacti -infile $1 | grep '  Access time (ns): ' | xargs python -c 'import sys; print(sys.argv[4])')
rm $1
cd ../tuner/ > /dev/null

if [[ -z $AREA ]]; then
    echo 'ERROR'
else
    echo $AREA $ACCT
fi

exit 0
