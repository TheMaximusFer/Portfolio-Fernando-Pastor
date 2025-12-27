#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

if [ $# -ne 3 ]; then
    echo "Numero incorrecto de parametros" >&2
    exit 1
fi

opcion="$1"
usuarios="$2"
maquinas="$3"

while read -r ip || [ -n "$ip" ]; do
    # Verificamos conectividad
    if ! ping -c 1 -W 1 "$ip" &>/dev/null; then
        echo "$ip no es accesible" >&2
        continue
    fi
    
    # Copiamos archivo a la máquina
    scp -i ~/.ssh/id_as_ed25519 practica_3.sh "$usuarios" "as@$ip:~/" &>/dev/null
    
    # Ejecutamos script remoto
    ssh -i ~/.ssh/id_as_ed25519 -n "as@$ip" \
        "sudo bash ~/practica_3.sh $opcion ~/$usuarios && rm ~/practica_3.sh ~/$usuarios"
    
done < "$maquinas" 

exit 0