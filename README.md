# COUNTER STRIKE 2D 

### Taller de programación - grupo 6 - cátedra: Veiga

### Integrantes

- [Federico Honda](https://github.com/federicohonda) - 110766
- [Martina Gualdi](https://github.com/martinagualdi) - 110513
- [Santiago Varga](https://github.com/SantiagoVarga) - 110561
- [Thiago Baez](https://github.com/thiagobaez) - 110703

### Corrector
[Leonardo Giampieri Mutti](https://github.com/leogm99)

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


### Manual de Usuario — Controles del Juego

## Movimiento del Jugador

- **W** — Mover arriba  
- **A** — Mover izquierda  
- **S** — Mover abajo  
- **D** — Mover derecha  
- **W+A**, **W+D**, **S+A**, **S+D** — Movimiento diagonal

## Disparo

- **Mouse** — Apuntar (el ángulo de disparo sigue al puntero)
- **Click izquierdo** — Disparar el arma
- **Rueda del mouse (scroll abajo)** — Cambiar de arma

## Interacciones

- **E** — Plantar o desactivar bomba  
  - Mantener presionada para accionar la bomba, soltar para detener
- **F** — Levantar (tomar) objetos del piso (por ejemplo, armas)
- **G** — Dropear (tirar) el arma actual

## Compra de Armas y Munición

- **B** — Abrir menú de compra (solo cuando está permitido)
    - **1** — Comprar AK47  
    - **2** — Comprar M3  
    - **3** — Comprar AWP  
    - **,** (coma) — Comprar balas de arma primaria  
    - **.** (punto) — Comprar balas de arma secundaria  
    - **ESC** — Cerrar menú de compra sin comprar

> **Nota:**  
> Si el menú de compra está abierto, se detiene el movimiento del jugador hasta cerrarlo.

---

## Selección de Skin

- Al inicio de la partida, elegí tu skin con:
    - **1** — CT: Seal Force / TT: Pheonix
    - **2** — CT: German GSG-9 / TT: L337 Krew
    - **3** — CT: UK SAS / TT: Artic Avenger
    - **4** — CT: French GIGN / TT: Guerrilla


## Estadísticas de la Partida

- **TAB** — Mantener presionado para mostrar las estadísticas (scoreboard).  
  - Solta TAB para ocultarlas.

## Aviso y Confirmación de Salida

- **Q** — Mostrar / ocultar aviso de desconexión (confirmar antes de salir)
- **ENTER** (cuando el aviso de desconexión está activo) — Confirmar salida
- **Cerrar ventana** — Salir del juego


## Otros

- Si el menú de compra está abierto, solo se puede comprar hasta que se cierre (con ESC o comprando).
- Si el jugador muere, no podrá moverse ni interactuar hasta la próxima ronda.

## Resumen Rápido

| Acción                      | Tecla / Botón        |
|-----------------------------|----------------------|
| Mover                       | W / A / S / D        |
| Disparar                    | Click izquierdo      |
| Apuntar                     | Mouse                |
| Cambiar arma                | Rueda mouse (abajo)  |
| Comprar                     | B (abrir menú)       |
| Confirmar compra            | 1, 2, 3, ,, .        |
| Dropear arma                | G                    |
| Levantar objeto             | F                    |
| Plantar/desactivar bomba    | E                    |
| Scoreboard                  | TAB                  |
| Seleccionar skin            | 1, 2, 3, 4           |
| Aviso de salida             | Q                    |
| Confirmar salida            | ENTER                |

# Lógica General del Juego


## Estructura de Partida

- **Inicio:**  
  El juego comienza cuando hay suficientes jugadores en ambos equipos (definido en el archivo de configuración).
- **Rondas:**
  El juego se divide en rondas. Cada ronda inicia con los jugadores en su zona de spawn y un tiempo de compra de armas limitado.
- **Compra:**  
  Durante el tiempo de compra, los jugadores pueden adquirir armas y munición sólo dentro de la zona de compra. No hace falta que el usuario abra la tienda para comprar balas.


## Condiciones de Finalización de Ronda

La ronda termina cuando ocurre uno de estos eventos:
- Todos los jugadores de un equipo han muerto:
  - Si solo quedan CT vivos y la bomba no fue plantada, ganan los CT.
  - Si quedan TT vivos, ganan los TT.
- **Bomba:**
  - Si la bomba es plantada y explota, ganan los TT.
  - Si la bomba es desactivada a tiempo por un CT, ganan los CT.


Al finalizar la ronda:
- Se actualizan los puntajes.
- Se muestra el ganador.
- Hay un intervalo antes de la próxima ronda.


## Lógica de la Bomba

- Solo un jugador TT con la bomba puede plantarla en la zona designada.
- Plantar/desactivar requiere mantener la acción durante un tiempo.
- Al plantar, la bomba inicia un timer para detonar.
- Los CT pueden desactivarla en la zona correspondiente.
- Si explota, daña a los CT cercanos.
- Si es desactivada antes de explotar, ganan los CT.


## Dinámica de Armas y Balas

- Los jugadores pueden:
  - Comprar armas/munición solo en tiempo y zona de compra.
  - Cambiar, dropear y levantar armas.
  - Disparar armas (AK47, M3, AWP, cuchillo, etc).
- Las balas se mueven por el mapa y chequean colisiones contra paredes y jugadores.
- Los jugadores muertos dejan caer su arma principal.


## Gestión de Jugadores

- Los jugadores se asignan alternando entre CT y TT para balancear equipos.
- Al morir, el jugador pierde su arma principal. Esta queda en forma de drop en el piso. Otros jugadores pueden levantarla antes que se termine la ronda.
- Al iniciar una nueva ronda, todos los jugadores reviven y vuelven a sus spawns.
- Si un jugador se desconecta, es eliminado y se libera la memoria.

## Cambio de Bandos

- Tras cierta cantidad de rondas, los equipos se intercambian:
  - CT pasa a TT y viceversa.
  - Se reubican en el spawn del nuevo bando.

## Fin de Partida

- Cuando se alcanza el máximo de rondas , la partida finaliza.
- Se informa el estado final a todos los jugadores.


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

# Extras

### Manuales e Informes (Técnicos y de uso)
Dentro de la carpeta 'documentation' se encuentran los 5 manuales e informes del juego.

- 'Manual de Instalación'.
- 'Manual de Uso'.
- 'Informe Técnico'.
- 'Informe del Proyecto'.
- 'Enunciado CS2D'.

### Créditos
Se han usado las siguiente bibliotecas desarrolladas por [Di Paola, Martin](https://www.github.com/eldipa):

- 'Socket'.
- 'Resolver'.
- 'ResolverError'.
- 'LibError'.
- 'Queue'.
- 'Thread'.

### Interfaz gráfica del juego
<img width="960" alt="image" src="https://github.com/user-attachments/assets/21c8d0fa-85dc-4929-9945-fd419d719437" />

### Interfaz gráfica del editor
<img width="805" alt="image" src="https://github.com/user-attachments/assets/42b44441-c74b-422f-902b-939c010424fa" />

