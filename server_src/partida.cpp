#include "partida.h"

Partida::Partida(const int codigo):
    codigo_partida(codigo),
    queue_comandos(),
    jugadores_queues(),
    gameloop(queue_comandos, jugadores_queues) {
        gameloop.start();
}

void Partida::agregar_jugador(int id, Queue<Snapshot>& queue_enviadora) {
    jugadores_queues.agregar_queue(queue_enviadora, id);
    gameloop.agregar_jugador_a_partida(id);
}