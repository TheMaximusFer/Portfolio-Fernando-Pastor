#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

HOME="$HOME"
bool_nuevo_creado=0
dir_seleccionado=""
min_mtime=""

for d in "$HOME"/bin???; do
    if [ -d "$d" ]; then
        mtime=$(stat -c %Y "$d")
        if [ -z "$min_mtime" ] || [ "$mtime" -lt "$min_mtime" ]; then
            min_mtime=$mtime
            dir_seleccionado="$d"
        fi
    fi
done

if [ -n "$dir_seleccionado" ]; then
    destino="$dir_seleccionado"
else
    destino=$(mktemp -d "$HOME/binXXX")
    bool_nuevo_creado=1
fi

if [ $bool_nuevo_creado -eq 1 ]; then
    echo "Se ha creado el directorio $destino"
    echo "Directorio destino de copia: $destino"
else
    echo "Directorio destino de copia: $destino"
fi

contador=0

for f in ./*; do
    if [ -f "$f" ] && [ -x "$f" ]; then
        cp "$f" "$destino"
        echo "$f ha sido copiado a $destino"
        ((contador++))
    fi
done

if [ $contador -gt 0 ]; then
    echo "Se han copiado $contador archivos"
else
    echo "No se ha copiado ningun archivo"
fi
