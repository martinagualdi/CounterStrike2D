#include "partida.h"

Partida::Partida(std::string creador_username, const int codigo, std::string yaml_partida) :
    codigo_partida(codigo),
    creador(creador_username),
    queue_comandos(),
    jugadores_queues(),
    activa(true),
    gameloop(queue_comandos, jugadores_queues, activa, yaml_partida),
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

bool Partida::eliminar_jugador(int id_jugador) {
    jugadores_queues.eliminar_queue(id_jugador);
    gameloop.eliminar_jugador_de_partida(id_jugador);
    cant_jugadores--;
    std::cout << "[Partida] Se elimino jugador de id: " << id_jugador << std::endl;
    return cant_jugadores == 0;
}

Partida::~Partida() {
    std::cout << "[Partida] Se esta eliminando la partida con codigo: " << codigo_partida << std::endl;
    //queue_comandos.close();
    // gameloop.stop();
    activa = false;
    gameloop.join();
    std::cout << "[Partida] Se termino la eliminacion de la partida con codigo: " << codigo_partida << std::endl;

}