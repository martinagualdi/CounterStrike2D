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
    Queue<ComandoDTO> queue_comandos;
    ListaQueues jugadores_queues;
    GameLoop gameloop;
    std::mutex mtx;

public:
    Partida(const int codigo);

    int get_codigo_partida() {return this->codigo_partida;};

    void agregar_jugador(int id, Queue<Snapshot>& queue_enviadora);

    Queue<ComandoDTO>& get_queue() { return this->queue_comandos; }
};

#endif
