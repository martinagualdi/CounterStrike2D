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

Para marcar tanto la zonas de inicio de CT y TT como para la zona de plantacion de bombas el editor cuenta con un boton **Marcar zona** una vez presionado se habilita para marcar una zona cualquiera con el cursor. Al finalizar la seleccion se consultara a que zona hace referencia lo marcado (inicio_ct, inicio_tt o zona_bombas). 

**IMPORTANTE:** al finalizar de marcar una zona, el modo del mismo se desactiva, por lo que para crear otra zona se deberá volver a presionar el botón. De esta maner se permite:

- Hacer **click** sobre la zona y moverla al lugar que desees. 

- Presionar **tecla supr** para eliminar la zona si asi quisieras.

- Hacer **click en la esquina inferior derecha** para redimencionar la zona. 

- Hacer **doble click** en el centro de la zona para cambiarle el tipo a la misma.


