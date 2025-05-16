#ifndef PARTIDA_H
#define PARTIDA_H

#include <vector>
#include <string>
#include <list>
#include <mutex>
#include "comando_dto.h"
#include "clienthandler.h"
#include "lista_queues.h"
#include "../common_src/queue.h"

class Partida{
private: 
    std::string codigo_partida;
    std::list<ClientHandler*> jugadores;
    Queue<ComandoDTO> queue_comandos;
    ListaQueues jugadores_queues;
    std::mutex mtx;

public:
    Partida(const std::string& codigo);

    // "Static" para validarlo por fuera antes de crearlo, asi no hay dos partidas con el mismo codigo
    static std::string generar_codigo_partida();

    std::string get_codigo_partida() {return this->codigo_partida;};


};

#endif
