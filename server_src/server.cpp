#include "server.h"

#include <string>

Server::Server(const char* servname):
        aceptador(servname, partidas_activas) {}

void Server::comenzar_a_aceptar() { aceptador.start(); }


void Server::leer_entrada() {
    std::string linea;
    while (getline(std::cin, linea)) {
        if (linea == EXIT_SERVER)
            break;
    }
}

void Server::start() {
    comenzar_a_aceptar();
    leer_entrada();
}

Server::~Server() {
    aceptador.dejar_de_aceptar();
}
