#ifndef PARTIDA_H
#define PARTIDA_H

#include <vector>
#include <string>
#include <list>
#include <mutex>
#include "../common_src/comando_dto.h"
#include "lista_queues.h"
#include "gameloop.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"

class Partida{
private: 
    int codigo_partida;
    std::string creador;
    Queue<ComandoDTO> queue_comandos;
    ListaQueues jugadores_queues;
    GameLoop gameloop;
    int cant_jugadores = 0;
    int cant_min_ct;
    int cant_min_tt;
    std::mutex mtx;

public:
    Partida(std::string creador_username, const int codigo, std::string yaml_partida);

    bool puedo_agregar_jugador() { return (cant_jugadores < (cant_min_ct + cant_min_tt)); }
    
    std::string get_creador() {return this->creador;};

    int get_codigo_partida() {return this->codigo_partida;};

    bool agregar_jugador(int id, std::string nombre, Queue<Snapshot>& queue_enviadora);

    Queue<ComandoDTO>& get_queue() { return this->queue_comandos; }

    std::string obtener_mapa();
};

#endif
