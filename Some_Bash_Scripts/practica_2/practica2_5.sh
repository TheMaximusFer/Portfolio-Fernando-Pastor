#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

echo "Introduzca el nombre de un directorio: "
read path
numFich=0
numDir=0
if [ -d "$path" ]
then
    for i in "$path"/* 
    do
        if [ -f "$i" ]
        then
            numFich=$(($numFich + 1))
        elif [ -d "$i" ]
        then
            numDir=$(($numDir + 1))
        fi
    done
    echo "El numero de ficheros y directorios en $path es de $numFich y $numDir, respectivamente"
else
    echo "$path no es un directorio"
fi
