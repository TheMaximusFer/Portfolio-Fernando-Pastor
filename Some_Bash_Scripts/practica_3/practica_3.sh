#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A  
#897113, Pastor Peralta, Fernando, M, 3, A

if [ "$(id -u)" -ne 0 ]; then
    echo "Este script necesita privilegios de administracion" >&2
    exit 1
fi

if [ $# -ne 2 ]; then
    echo "Numero incorrecto de parametros" >&2
    exit 1
fi

opcion="$1"
fichero="$2"

if [[ "$opcion" != "-a" && "$opcion" != "-s" ]]; then
    echo "Opcion invalida" >&2
    exit 1
fi
oldIFS=$IFS
IFS=','

# Procesar según opción
case "$opcion" in
    -a)
        while  read -r login_name contrasena nombre_completo || [ -n "$login_name" ]; do
            if [[ -z "$login_name" || -z "$contrasena" || -z "$nombre_completo" ]]; then
                echo "Campo invalido" >&2
                exit 1
            fi

            if id -u "$login_name" &>/dev/null; then
                echo "El usuario $login_name ya existe"
            else
                useradd -U -m -k /etc/skel -K PASS_MAX_DAYS=30 -K UID_MIN=1815 -c "$nombre_completo" "$login_name"
                if [ $? -eq 0 ]; then
                    echo "$login_name:$contrasena" | chpasswd &>/dev/null
                    usermod -U "$login_name" &>/dev/null
                    echo "$nombre_completo ha sido creado"
                fi
            fi
        done < "$fichero"
        ;;
    -s)
        mkdir -p /extra/backup || exit 1

        while read -r linea kk;
        do
            login_name=$linea;
            [[ -z "$login_name" ]] && continue

            if id -u "$login_name" &>/dev/null
            then
                home_dir=$(getent passwd "$login_name" | cut -d: -f6)
                home_dir=${home_dir:-"/home/$login_name"}

                if tar -cf "/extra/backup/$login_name.tar" -C "$home_dir" . &>/dev/null 
                then
                    userdel -r "$login_name" &>/dev/null
                else
                    echo "Backup fallido para $login_name, no se borra." >&2
                fi
            fi
        done < "$fichero"
        ;;
esac
IFS=$oldIFS
exit 0
