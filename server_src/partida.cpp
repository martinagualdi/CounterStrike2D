#include "partida.h"

Partida::Partida(const std::string& codigo):
    codigo_partida(codigo),
    jugadores(),
    queue_comandos(),
    jugadores_queues() {}

void Partida::agregar_jugador(Socket&& peer) {
    ClientHandler* client = new ClientHandler(std::move(peer), queue_comandos, jugadores.size());
    jugadores.push_back(client);
    jugadores_queues.agregar_queue(client->get_queue(), jugadores.size());
    client->iniciar();
}