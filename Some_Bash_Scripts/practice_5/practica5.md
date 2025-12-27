
# Práctica 5

## Preparación de discos y sistemas de archivos

### 1. Añadir disco adicional
Antes de arrancar nuestras máquinas virtuales (MV), les añadimos un disco adicional de **32MB** a cada una.

### 2. Instalación de software necesario

```bash
sudo apt install lvm2
sudo systemctl list-units
sudo apt install gdisk
sudo apt install parted
```

### 3. Configuración del nuevo disco

#### a. Verificamos los discos conectados:

```bash
sudo fdisk -l
```

#### b. Creamos las particiones con `gdisk`:

```bash
sudo gdisk /dev/sdb
```

Luego, en el menú interactivo, introducimos los siguientes comandos:

```
o        # (opcional) Crea una nueva tabla GPT
n        # Nueva partición
↲        # Número de partición por defecto
↲        # Primer sector por defecto
+16MB    # Tamaño de la primera partición
↲        # Confirmamos

n        # Segunda partición
↲        # Número de partición por defecto
↲        # Primer sector por defecto
↲        # Usar todo el espacio restante
↲        # Confirmamos

w        # Guardamos y salimos
```

### 4. Comprobación de alineamiento

```bash
sudo parted /dev/sdb align-check optimal 1
sudo parted /dev/sdb align-check optimal 2
```

### 5. Crear sistemas de archivos

```bash
sudo mkfs -t ext3 /dev/sdb1
sudo mkfs -t ext4 /dev/sdb2
```

### 6. Crear puntos de montaje

```bash
cd /media
sudo mkdir ds1
sudo mkdir ds2
```

### 7. Configurar montaje automático en `/etc/fstab`

Editamos el archivo:

```bash
sudo vi /etc/fstab
```

Y añadimos al final:

```
/dev/sdb1    /media/ds1  ext3   defaults 0 0
/dev/sdb2    /media/ds2  ext4   defaults 0 0
```

### 8. Reiniciamos para aplicar cambios

```bash
sudo shutdown -r now
```

### 9. Comprobamos los montajes

```bash
sudo mount /dev/sdb1
sudo mount /dev/sdb2
```

---

## Configuración de red y claves SSH

### 1. Cambiar configuración de red en VirtualBox

Modificamos el **Adaptador 1** de ambas máquinas para usar la red **Host Only**.

### 2. Configuración de interfaces de red

Editamos el archivo:

```bash
sudo vi /etc/network/interfaces
```

Comentamos las líneas relacionadas con `enp0s3` y añadimos (ajustando la IP para cada MV):

Para la **primera MV**:

```
auto enp0s3
iface enp0s3 inet static
    address 192.168.58.2
    netmask 255.255.255.0
```

Para la **segunda MV**:

```
auto enp0s3
iface enp0s3 inet static
    address 192.168.58.3
    netmask 255.255.255.0
```

Reiniciamos la red:

```bash
sudo systemctl restart networking
```

### 3. Configurar acceso SSH sin contraseña

Desde el **host**, generamos una clave:

```bash
ssh-keygen -b 4096 -t ed25519
```

Copiamos la clave pública a ambas máquinas:

```bash
scp ~/.ssh/id_ed25519.pub as@192.168.58.2:~/
scp ~/.ssh/id_ed25519.pub as@192.168.58.3:~/
```

En cada MV, agregamos la clave al archivo `authorized_keys`:

```bash
touch ~/.ssh/authorized_keys
cat ~/id_ed25519.pub >> ~/.ssh/authorized_keys
```

Además, editamos el archivo de sudoers para evitar pedir contraseña al usuario `as`:

```bash
sudo visudo
```

Añadimos al final:

```
as ALL=(ALL) NOPASSWD:ALL
```

### 4. Ejecutamos los scripts desde el host

```bash
./practica5_parte2.sh 192.168.58.2
./practica5_parte2.sh 192.168.58.3
```

---

## Gestión con LVM

### 1. Añadir disco extra

Añadimos a una de las MVs un nuevo disco virtual de **32MB**.

Listamos los discos para identificarlo:

```bash
sudo fdisk -l
```

### 2. Crear partición y marcarla como LVM

```bash
sudo gdisk /dev/sdc
```

En el menú interactivo:

```
n    # Nueva partición
↲    # Número por defecto
↲    # Inicio por defecto
↲    # Todo el espacio
↲    # Confirmar
w        # Guardar cambios
```

Activamos la partición como LVM:

```bash
sudo parted /dev/sdc set 1 lvm on
```

### 3. Crear grupo de volúmenes (VG)

Verificamos los grupos existentes:

```bash
sudo vgdisplay
```

Creamos un nuevo VG:

```bash
sudo vgcreate vg_p5 /dev/sdc1
```

### 4. Preparar el sistema para nuevo montaje

Subimos el script desde el host:

```bash
scp practica5_parte3_vg.sh as@192.168.58.2:~/
```

Editamos `/etc/fstab` y eliminamos las líneas relacionadas con `/media/ds1` y `/media/ds2`.

Reiniciamos la máquina:

```bash
sudo shutdown -r now
```

Ejecutamos el script de creación del VG:

```bash
sudo ./practica5_parte3_vg.sh vg_p5 /dev/sdb1 /dev/sdb2
```

### 5. Crear volúmenes lógicos (LV)

Subimos el segundo script y el archivo de entrada:

```bash
scp practica5_parte3_lv.sh as@192.168.58.2:~/
scp inputLV.txt as@192.168.58.2:~/
```

Ejecutamos el script:

```bash
sudo ./practica5_parte3_lv.sh < inputLV.txt
```

---

