#include "partida.h"

Partida::Partida(const std::string& codigo):
    codigo_partida(codigo),
    jugadores(),
    queue_comandos(),
    jugadores_queues() {}

std::string Partida::generar_codigo_partida() {
    std::string codigo_partida;
    const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 6; ++i) {
        codigo_partida += caracteres[rand() % (sizeof(caracteres) - 1)];
    }
    return codigo_partida;
}