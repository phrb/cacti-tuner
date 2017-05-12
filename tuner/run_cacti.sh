#! /bin/bash

cd ../cacti_hp > /dev/null
ACCT=$(./cacti -infile $1 | grep '  Access time (ns): ' | xargs python -c 'import sys; print(sys.argv[4])')
rm $1
cd ../tuner/ > /dev/null

if [[ -z $ACCT ]]; then
    echo 'ERROR'
else
    echo $ACCT
fi

exit 0
