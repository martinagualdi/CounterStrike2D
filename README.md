# Manual de usuario

**Importante:** el usuario previo a la compilacion debe instalar:
```sh
sudo apt install qt6-base-dev qt6-multimedia-dev
sudo apt-get install libyaml-cpp-dev
```

###### **Para compilar:**
```sh
make
```

###### **Para borrar los ejecutables:**
```sh
make clean
```

###### **Para ejecutar el servidor:**
```sh
./server configuracion.yaml
```
El archivo de configuración contiene el puerto al cual conectarse, predefinido 8080

###### **Para ejecutar el cliente:**
```sh
./client
```
Una vez ejecutado deberá ingresar **host**, **port** y **username** en la pantalla de inicio para conectarse. 

###### **Para ejecutar el editor:**
```sh
./taller_editor
```

#### Movimientos del jugador
Para mover al jugador podes usar:

- Teclado: W(arriba), S(abajo), A(derecha), D(izquierda). 

- Mouse: para rotar el frente del jugador. 


#### Uso del editor de niveles/mapas

El editor cuenta con un lobby el cual tiene dos opciones **editar** o **crear** un mapa.

En ambos casos tener las siguientes consideraciones:

- Para setear el fondo basta con hacer **click** sobre el fondo elegido. 

- Para posicionar un elemento se utiliza **drag and drop**, arrastrar la imagen a la posición deseada. 

Una vez posicionado un objeto se podrá mover (haciendo click sobre el mismo y arrastrando) y eliminar (haciendo click sobre el mismo y presionando la tecla de delete). 

Para guardar un mapa se debe presionar el boton de **Guardar** y seleccionar la carpeta **client_src/gfx/mapas**. 

Al momento de editar un mapa ya existe se abre automaticamente esa carpeta para que se seleccione el mapa deseado, luego de finalizar la edición se debe guardar con el mismo nombre para reemplazar el archivo existente por el nuevo que contiene los cambios realizados. 


