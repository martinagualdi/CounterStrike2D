#include "gameloop.h"

GameLoop::GameLoop(Queue<ComandoDTO>& queue_comandos, ListaQueues& queues_jugadores) :
    queue_comandos(queue_comandos),
    queues_jugadores(queues_jugadores),
    jugadores(),
    activo(true){}

void GameLoop::agregar_jugador_a_partida(const int id) {
    Jugador* jugador = new Jugador();
    jugadores.push_back(jugador);
}

void GameLoop::run() {
    while(activo){
        try {
            ComandoDTO comando;
            if (queue_comandos.try_pop(comando)){
                std::string movimiento;
                if (comando.movimiento == 1) {
                    movimiento = "W";
                } else if (comando.movimiento == 2) {
                    movimiento = "D";
                } else if (comando.movimiento == 3) {
                    movimiento = "S";
                } else if (comando.movimiento == 4) {
                    movimiento = "A";
                }
                std::cout << "Comando recibido: El jugador (id: " << comando.id_jugador << ") hizo el comando: " << movimiento << "\n";
            }
            /*W,A,S,D = 1, 2, 3, 4*/
            for (Jugador* jugador : jugadores) {
                /*
                ACTUALIZO LA POSICION DEL JUGADOR DEPENDIENDO EL COMANDO Y EL ID QUE VIENE EN EL
                */
                if (jugador->comparar_id(comando.id_jugador)) {
                    switch (comando.movimiento) {
                        case 1:
                            jugador->setY(jugador->getY() - 1);
                            break;
                        case 2:
                            jugador->setX(jugador->getX() + 1);
                            break;
                        case 3:
                            jugador->setY(jugador->getY() + 1);
                            break;
                        case 4:
                            jugador->setX(jugador->getX() - 1);
                            break;
                    }
                }
            }
            Snapshot snapshot(jugadores);
            queues_jugadores.broadcast(snapshot);
        } catch (const ClosedQueue&) {
            break;
        }
    }
}