#!/bin/bash
#911715, Sainz Sampedro, Adriana, M, 3, A
#897113, Pastor Peralta, Fernando, M, 3, A

while IFS=',' read -r VG_NAME LV_NAME SIZE FS_TYPE MOUNT_POINT; do
    LV_PATH="/dev/$VG_NAME/$LV_NAME"
    if sudo vgdisplay "$VG_NAME" >/dev/null 2>&1; then
        if sudo lvdisplay "$LV_PATH" &>/dev/null; then
            echo "Extendiendo volumen lógico $LV_PATH a $SIZE..."
            sudo lvextend -L "$SIZE" "$LV_PATH"
            sudo resize2fs "$LV_PATH"  # Solo para ext4
        else
            echo "Creando volumen lógico $LV_NAME en $VG_NAME..."
            sudo lvcreate -L "$SIZE" -n "$LV_NAME" "$VG_NAME"
            echo "Formateando con $FS_TYPE..."
            sudo mkfs -t "$FS_TYPE" "$LV_PATH"
            echo "Creando punto de montaje en $MOUNT_POINT..."
            sudo mkdir -p "$MOUNT_POINT"
            sudo mount "$LV_PATH" "$MOUNT_POINT"
            echo "Añadiendo a /etc/fstab..."
            FS_LINE="$LV_PATH $MOUNT_POINT $FS_TYPE defaults 0 2"
            echo "$FS_LINE" | sudo tee -a /etc/fstab > /dev/null
        fi
    else
        echo "$VG_NAME no existe"
        exit 1
    fi
done