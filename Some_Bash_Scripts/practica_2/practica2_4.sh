#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

echo "Introduzca una tecla: "
read char

case $char in 
 [A-Za-z]*)
 echo "${char:0:1} es una letra";;
 [0-9]*)
 echo "${char:0:1} es un numero";;
 *)
 echo "${char:0:1} es un caracter especial";;
esac
