#include "partida.h"

Partida::Partida(const std::string& codigo):
    codigo_partida(codigo),
    jugadores(),
    queue_comandos(),
    jugadores_queues(),
    gameloop(queue_comandos, jugadores_queues) {
        gameloop.start();
}

void Partida::agregar_jugador(Socket&& peer, int id) {
    ClientHandler* client = new ClientHandler(std::move(peer), queue_comandos, id);
    jugadores.push_back(client);
    jugadores_queues.agregar_queue(client->get_queue(), id);
    client->iniciar();
    gameloop.agregar_jugador_a_partida(id);
}