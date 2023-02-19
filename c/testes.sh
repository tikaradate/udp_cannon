#!/usr/bin/env bash
# BASH DE TESTES

if [ "$1" == "" ] || [ "$2" == "" ]; then
    echo "1 uso: /tests.sh <host> <porta>"
    exit 1
fi

make
# invocar varias instancias de cliente
# analisar os dados :)

for qt in 10 100 1000 10000 100000;
do 
    echo $qt
    ./client $1 $2 $qt
    sleep 3
done

make purge
