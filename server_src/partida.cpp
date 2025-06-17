#include "partida.h"

Partida::Partida(std::string creador_username, const int codigo, std::string yaml_partida) :
    codigo_partida(codigo),
    creador(creador_username),
    queue_comandos(),
    jugadores_queues(),
    gameloop(queue_comandos, jugadores_queues, yaml_partida),
    cant_jugadores(0),
    cant_min_ct(Configuracion::get<int>("cantidad_min_ct")),
    cant_min_tt(Configuracion::get<int>("cantidad_min_tt")){
        gameloop.start();
}


bool Partida::agregar_jugador(int id, std::string nombre, Queue<Snapshot>& queue_enviadora) {
    if (!puedo_agregar_jugador()) {
        return false;
    }
    jugadores_queues.agregar_queue(queue_enviadora, id);
    gameloop.agregar_jugador_a_partida(id, nombre);
    cant_jugadores++;
    
    return true;
}

std::string Partida::obtener_mapa() {
    return gameloop.mapa_en_estado_inicial();
}