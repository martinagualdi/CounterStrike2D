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

##### Movimientos del jugador
Para mover al jugador podes usar:

- Teclado: W(arriba), S(abajo), A(derecha), D(izquierda). 

- Mouse: para rotar el frente del jugador. 


