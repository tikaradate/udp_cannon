#!/usr/bin/env bash
# BASH DE TESTES

if [ "$1" == "" ] || [ "$2" == "" ]; then
    echo "1 uso: /tests.sh <host> <porta>"
    exit 1
fi

make
# invocar varias instancias de cliente
# analisar os dados :)

for qt in 1000 10000 100000 1000000;
do 
    for i in $(seq 15);
    do
        echo $qt
        ./client $1 $2 $qt
        sleep 3
    done
done

make purge
