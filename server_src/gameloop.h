#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "../common_src/thread.h"
#include "../common_src/queue.h"
#include "../common_src/jugador.h"
#include "../common_src/snapshot.h"
#include "comando_dto.h"
#include "lista_queues.h"

class GameLoop : public Thread {
private:
    Queue<ComandoDTO>& queue_comandos;
    ListaQueues& queues_jugadores;
    std::vector<Jugador*> jugadores;
    std::atomic<bool> activo;
public:
    explicit GameLoop(Queue<ComandoDTO>& queue_comandos, ListaQueues& queues_jugadores);

    void agregar_jugador_a_partida(const int id);

    virtual void run() override;
};

#endif 