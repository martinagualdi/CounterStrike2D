#include "server.h"

#include <string>

Server::Server(const char* servname):
        queue_juego(50),
        lista_queues(),
        aceptador(servname, queue_juego, lista_queues),
        procesador_de_mensajes(queue_juego, lista_queues) {}

void Server::comenzar_a_aceptar() { aceptador.start(); }

void Server::comenzar_a_procesar() { procesador_de_mensajes.start(); }

void Server::leer_entrada() {
    std::string linea;
    while (getline(std::cin, linea)) {
        if (linea == EXIT_SERVER)
            break;
    }
}

void Server::start() {
    comenzar_a_aceptar();
    comenzar_a_procesar();
    leer_entrada();
}

Server::~Server() {
    aceptador.dejar_de_aceptar();
}
