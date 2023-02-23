#!/usr/bin/env bash

if [ "$1" == "" ] || [ "$2" == "" ]; then
    echo "1 uso: /tests.sh <host> <porta>"
    exit 1
fi



python3 client.py $1 $2 1000

for qt in 1000 100000;
do 
    for i in $(seq 15);
    do
        echo $qt
        python3 client.py $1 $2 $qt
        sleep 5
    done
done
