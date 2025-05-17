#ifndef PARTIDA_H
#define PARTIDA_H

#include <vector>
#include <string>
#include <list>
#include <mutex>
#include "comando_dto.h"
#include "clienthandler.h"
#include "lista_queues.h"
#include "gameloop.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"

class Partida{
private: 
    std::string codigo_partida;
    std::list<ClientHandler*> jugadores;
    Queue<ComandoDTO> queue_comandos;
    ListaQueues jugadores_queues;
    GameLoop gameloop;
    std::mutex mtx;

public:
    Partida(const std::string& codigo);


    std::string get_codigo_partida() {return this->codigo_partida;};

    void agregar_jugador(Socket&& peer, int id);


};

#endif
