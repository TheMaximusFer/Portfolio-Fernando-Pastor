#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

if [ $# -ne 1 ]; then
    echo "Uso: ./practica_parte2.sh <IP>"
    exit 1
fi

IP=$1
SSH_CMD="ssh as@$IP"

echo ">>> Discos duros disponibles y sus tamaños (sfdisk -s):"
SSH_CMD "sudo sfdisk -s 2>/dev/null" | grep -v total

echo ">>> Particiones y sus tamaños (sfdisk -l):"
SSH_CMD "sudo sfdisk -l 2>/dev/null" | grep -E '^/dev/'

echo ">>> Información de montaje de sistemas de ficheros (df -hT, excluyendo tmpfs):"
SSH_CMD "df -hT | grep -v tmpfs | grep -v 'Filesystem'" #Quitamos encabezado