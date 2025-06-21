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
###### **Para correr el instalador:**
```sh
make install
```
###### **Para eliminar instalaciones:**
```sh
make uninstall
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

###### **Para ejecutar el editor:**
```sh
./taller_editor
```

#### Lobby
Una vez ejecutado el cliente se deberá ingresar **host**, **port** y **username** en la pantalla de inicio para conectarse. 
De esta manera, cuando el cliente ya este correctamente conectado se podrá:

- **Crear una partida:**

    Se le asignaŕa automaticamente un bando y el cliente podrá elegir su skin (correspondiente al equipo) haciendo click en la imagen deseada.
    Se podrá elegir el mapa en el cual se quiere jugar haciendo click en el mismo. 

- **Listar las partidas activas:**

    Aparecerá una lista de las partidas, para seleccionar una se deberá hacer click sobre la misma. 

- **Unirse a una partida ya existente:**

    Una vez seleccionada una lista del item anterior, presionar el boton de unirse. 
    Se le asignaŕa automaticamente un bando y el cliente podrá elegir su skin (correspondiente al equipo) haciendo click en la imagen deseada.
    Se incorporará automaticamente a la partida con el mapa elegido por quien haya creado la misma. 

#### Movimientos del jugador
Para mover al jugador podes usar:

- Teclado: **W**(arriba), **S**(abajo), **A**(derecha), **D**(izquierda). 

- Mouse: para rotar el frente del jugador. 

#### Comprar armas

Para comprar armas al inicio de la partida:
- Se debe estar dentro de la zona de compra, indicada con un carrito de compras en el HUD de la pantalla.
- Presionar tecla B.
- Seleccionar el número de arma correspondiente.
- Para comprar balas del arma primaria usar la coma (,)
- Para comprar balas del arma secundaria usar el punto (.)

Cabe aclarar que para comprar balas no hace falta abrir el mercado. Solo para comprar armas es necesario abrirlo (apretar B).
La idea de este mercado es simular el mismo sistema que usa el Counter-Strike real.

#### Uso del editor de niveles/mapas

El editor cuenta con un lobby el cual tiene dos opciones **editar** o **crear** un mapa.

En ambos casos tener las siguientes consideraciones:

- Para setear el fondo basta con hacer **click** sobre el fondo elegido. 

- Para posicionar un elemento se utiliza **drag and drop**, arrastrar la imagen a la posición deseada. 

- Para "pintar" el piso se cuenta con un boton **activar pincel piso** el cual al ser clickeado transforma el cursos en un pincel para los sprites de pisos, es decir, cuando clickeamos el boton se activa dicho modo luego hay que clickear sobre el piso deseado y pintar la pantalla. Para pintar lo que hay que hacer es hacer click en la zona donde queres comenzar a pintar y arrastrar el mouse presionado, una vez que se deja de presionar le mousee el modo se desactivará. 
**IMPORTANTE:** si el modo pincel no esta activado podemos colocar pisos individualmente como cualquier otro objeto. Ademas una vez pintado cada piso en su individualidad podrá ser movido o eliminado al igual que el resto de los elementos. 

Una vez posicionado un objeto se podrá mover (haciendo click sobre el mismo y arrastrando) y eliminar (haciendo click sobre el mismo y presionando la tecla de delete). 

Para guardar un mapa se debe presionar el boton de **Guardar** y se abrirá automaticamente la carpeta **editor/mapas**. Al guardar un mapa se guardarán dos cosas: un **.yaml** y un **.jpg**, este ultimo es una imagen que se usaŕa de previsualizacion para la seleccion de mapa en el cliente. 

Al confirmar el guardado, ambos archivos se copiarán automaticamente en **server_src/mapas_disponibles**. 
**OBS:** en caso de estar ejecutando el editor con la aplicacion generada por el instalador, los archivos tambien se copiarán en las rutas correspondientes para ser utilizadas tanto por el editor como por el server desde este modo sin necesidad de recompilar. 

Al momento de editar un mapa ya existe se abre automaticamente esa carpeta (del editor) para que se seleccione el mapa deseado (el .yaml). Al finalizar la edición se debe guardar con el mismo nombre para reemplazar ambos archivos existentes por los nuevos que contienen los cambios realizados. 

Para marcar tanto la zonas de inicio de CT y TT como para la zona de plantacion de bombas el editor cuenta con un boton **Marcar zona** una vez presionado se habilita para marcar una zona cualquiera con el cursor. Al finalizar la seleccion se consultara a que zona hace referencia lo marcado (inicio_ct, inicio_tt o zona_bombas). 

**IMPORTANTE 1:** un mapa no podrá ser guardado si:

- No cuenta con una zona de inicio TT (tampoco se permite crear mas de una).

- No cuenta con una zona de inicio CT (tampoco se permite crear mas de una).

- No cuenta con al menos una zona de BOMBAS (se permiten máximo dos zonas de este tipo).

**IMPORTANTE 2:** al finalizar de marcar una zona, el modo del mismo se desactiva, por lo que para crear otra zona se deberá volver a presionar el botón. De esta maner se permite:

- Hacer **click** sobre la zona y moverla al lugar que desees. 

- Presionar **tecla supr** para eliminar la zona si asi quisieras.

- Hacer **click en la esquina inferior derecha** para redimencionar la zona. 

- Hacer **doble click** en el centro de la zona para cambiarle el tipo a la misma.

#### Instalador

Al correr el debido comando el instalador hará 5 cosas:

1. make clean
2. make
3. Correr tests con valgrind
4. Instalar todas las dependencias
5. Posicionar 3 ejecutables en el escritorio con icono de aplicacion. 

###### Para ejecutar el juego: 

Hacer doble click en el icono del server y luego en el del cliente, es importante mantener el orden. De esta manera el juego funcionará igual que en modo developer. 

###### Para ejecutar el editor: 

Hacer doble click en el icono del editor, luego funcionará igual que en modo developer. 

**IMPORTANTE:** en caso de que los iconos de aplicacion aparezcan con una **cruz** y un mensaje de que no permiten ser lanzadas, hacer click derecho sobre los mismos y habilitar dicho permiso (**allow launching**).


