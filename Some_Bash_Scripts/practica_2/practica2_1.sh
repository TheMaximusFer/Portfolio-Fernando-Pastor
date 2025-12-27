#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A


echo "Introduzca el nombre del fichero: "

fichero=""
permisos=""

read fichero

if test -e "$fichero"
then
   permisos=""

   if test -r "$fichero"
   then
      permisos="r"
   else
      permisos="-"
   fi

   if test -w "$fichero"
   then
      permisos="$permisos""w"
   else
      permisos="$permisos""-"
   fi

   if test -x "$fichero"
   then
      permisos="$permisos""x"
   else
      permisos="$permisos""-"
   fi

   echo "Los permisos del archivo $fichero son: $permisos"
   
else
   echo "$fichero no existe"
fi
