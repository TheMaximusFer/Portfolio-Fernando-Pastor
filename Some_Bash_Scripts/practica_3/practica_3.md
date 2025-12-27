#911715,  Sainz Sampedro,  Adriana,  M,  3,  A  
#897113,  Pastor Peralta,  Fernando,  M,  3,  A
# Decisiones de Implementación

## Descripción General
Se ha desarrollado un script en bash que permite gestionar usuarios en un sistema Linux. Existen dos funcionalidades principales:
- Crear usuarios con la opción `-a`.
- Eliminar usuarios con la opción `-s`.

## Decisiones Clave

1. **Requerimiento de Privilegios de Administración:**
    - Se valida que el script se ejecute con privilegios de superusuario (`id -u`). Esto asegura que las operaciones de creación y eliminación de usuarios se realicen correctamente. Si no, el estado desalida es 1 (`exit 1`)

2. **Control de Argumentos:**
    - Se verifican el número de argumentos pasados al script (`if [ $# -ne 2 ]`)  y la validez de la opción (`[[ "$opcion" != "-a" && "$opcion" != "-s" ]]`).

3. **Gestor de Entrada (IFS):**
    - Se establece `IFS=','` para procesar los archivos CSV con campos separados por comas.

4. **Creación de Usuarios:**
    - Se valida la existencia previa del usuario:
     (`if id -u "$login_name" &>/dev/null; then echo "El usuario $login_name ya existe"`)
    - Se utiliza `useradd` con las siguientes opciones:
      - `-U` para crear un grupo con el mismo nombre del usuario.
      - `-m` para crear el directorio home.
      - `-k /etc/skel` para inicializar con esos ficheros.
      - `-K PASS_MAX_DAYS=30` para forzar la caducidad de la contraseña a 30 días.
      - `-K UID_MIN=1815` para asignar UIDs desde 1815 en adelante.
    - Se establece la contraseña mediante `chpasswd`.
    - Se reactiva la cuenta del usuario con `usermod -U`. Realmete no haría falta, peo se nos pide usarlo.

5. **Eliminación de Usuarios:**
    - Se crea un directorio `/extra/backup` para almacenar respaldos.
    - ## Descripción

    - `home_dir=$(getent passwd "$login_name" | cut -d: -f6)`:
    `getent passwd "$login_name"` obtiene la información del usuario.
    `cut -d: -f6` extrae el sexto campo (el directorio home).
    Asigna el directorio home a la variable `home_dir`.

    - `home_dir=${home_dir:-"/home/$login_name"}`:
    Si `home_dir` no tiene valor, se asigna un valor por defecto: `"/home/$login_name"`.

    - Antes de eliminar el usuario, se realiza un respaldo del directorio home usando `tar`.


6. **Gestor de Errores:**
    - Se manejan situaciones de error mediante la verificación de códigos de salida (`$?`) y mensajes de error redirigidos a `stderr`.


