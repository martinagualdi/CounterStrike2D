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
            queue_comandos.try_pop(comando);
            for (Jugador* jugador : jugadores) {
                /*
                ACTUALIZO LA POSICION DEL JUGADOR DEPENDIENDO EL COMANDO Y EL ID QUE VIENE EN EL
                */
            }
            /* ARMO SNAPSHOT Y SE LA ENVIO A TODOS, ALGO COMO LO COMENTADO ABAJO */
            // Snapshot snapshot(jugadores);
            // queues_jugadores.broadcast(snapshot);
        } catch (const ClosedQueue&) {
            break;
        }
    }
}