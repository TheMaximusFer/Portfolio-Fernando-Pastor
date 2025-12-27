#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

archivo="$1"

if [ "$#" -eq 1 ]; then
    if [ -f "$archivo" ]; then
        chmod ug+x "$archivo"
        stat -c "%A" "$archivo"
    else
        echo "$archivo no existe"
    fi
else
    echo "Sintaxis: practica2_3.sh <nombre_archivo>"
    exit 1
fi

