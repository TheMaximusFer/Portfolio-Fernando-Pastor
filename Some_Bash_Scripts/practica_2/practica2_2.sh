#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

for param in "$@";
do 
    if [ -f "$param" ];
    then
        echo $(more "$param")
    else
        echo "$param" no es un fichero
    fi
done
