#911715,  Sainz Sampedro,  Adriana,  M,  3,  A  
#897113,  Pastor Peralta,  Fernando,  M,  3,  A
# Configuración de Máquinas e Instalación de Claves

## 1. Preparación de las Máquinas en VirtualBox
 
### 1.1 Configuración de la Red Virtual
Para establecer la comunicación entre las máquinas virtuales y el host:
1. En VirtualBox, hemos ido a **Archivo → Herramientas → Administrador de red**.
2. Hemos creado una red de tipo **Host-Only-Network** con:
   - **IPv4 Address**: `192.168.56.1`
   - **IPv4 Network Mask**: `255.255.255.0`
   - **DHCP Server**: Deshabilitado

### 1.2 Creación de Máquinas Virtuales
1. Hemos creado o importado dos máquinas virtuales y las hemos nombrado:
   - `debian_as1`
   - `debian_as2`
2. Hemos usado la imagen de la asignatura con credenciales `root/toor` y `as/as` desde:
   - [Descarga de la imagen](https://cloud.unizar.es/index.php/s/w5PWERJxaMkFMTF)
3. Para ahorrar espacio, hemos generado clones diferenciales a partir de una máquina base `as_base`.

### 1.3 Configuración de Red en VirtualBox
Para cada máquina (`debian_as1` y `debian_as2`):
1. **Adaptador 1**: NAT (para acceso a Internet)
2. **Adaptador 2**: Host-Only Adapter → `VirtualBox Host-Only Ethernet Adapter`
3. Hemos verificado:
   - Que cada adaptador tenga una MAC Address diferente.
   - Que la casilla **Cable Connected** esté activada.

## 2. Configuración de Red en las Máquinas Servidor

Hemos editado el archivo `/etc/network/interfaces` para establecer direcciones estáticas:
```bash
sudo nano /etc/network/interfaces
```
Lo hemos configurado de la siguiente manera:
```ini
# Interfaz de loopback
auto lo
iface lo inet loopback

# Interfaz principal (estática)
auto eth1
iface eth1 inet static
  address 192.168.56.X
  netmask 255.255.255.0
  network 192.168.56.0
```
> **Nota**: Hemos reemplazado `X` por `11` para `debian_as1` y `12` para `debian_as2`.

Hemos aplicado los cambios con:
```bash
sudo systemctl restart networking
```
Hemos verificado la conexión entre máquinas:
```bash
ping 192.168.56.12  # Desde debian_as1 a debian_as2
ping 192.168.56.11  # Desde debian_as2 a debian_as1
```

## 3. Configuración de Sudo
Hemos permitido al usuario `as` ejecutar comandos sin contraseña:
```bash
sudo nano /etc/sudoers
```
Hemos añadido la línea:
```ini
as ALL=(ALL) NOPASSWD:ALL
```

## 4. Configuración de Acceso por SSH

### 4.1 Generación de Claves SSH en la Máquina Cliente
```bash
ssh-keygen -b 4096 -t ed25519 -f ~/.ssh/id_as_ed25519
```
Hemos copiado la clave pública a los servidores:
```bash
scp ~/.ssh/id_as_ed25519.pub as@192.168.56.11:~/
scp ~/.ssh/id_as_ed25519.pub as@192.168.56.12:~/
```

### 4.2 Configuración en los Servidores
1. Hemos creado el archivo `authorized_keys`:
   ```bash
   touch ~/.ssh/authorized_keys
   ```
2. Hemos agregado la clave pública:
   ```bash
   cat ~/id_as_ed25519.pub >> ~/.ssh/authorized_keys
   ```

### 4.3 Configuración de SSH
Hemos modificado el archivo de configuración SSH:
```bash
sudo nano /etc/ssh/sshd_config
```
Hemos actualizado las siguientes líneas:
```ini
PermitRootLogin no
PasswordAuthentication no
```
Hemos reiniciado el servicio SSH:
```bash
sudo systemctl reload ssh
```
