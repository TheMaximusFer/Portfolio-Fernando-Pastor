#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

if [ $# -lt 2 ]; then
    echo "Uso: $0 <nombre_grupo_volumen> <partición1> [partición2 ...]"
    exit 1
fi

VG_NAME=$1
shift

for PART in "$@"; do
    sudo umount "$PART" >/dev/null 2>&1
    sudo pvcreate "$PART"
    sudo vgextend "$VG_NAME" "$PART" && echo "VG $VG_NAME extendido con $PART"
done

sudo vgdisplay "$VG_NAME"
