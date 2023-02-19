#!/usr/bin/env bash

if [ "$1" == "" ] || [ "$2" == "" ]; then
    echo "1 uso: /tests.sh <host> <porta>"
    exit 1
fi

for qt in $(seq 20);
do 
    echo $qt
    python3 client.py $1 $2 100000
    sleep 3
done
