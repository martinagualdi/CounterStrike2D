#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <list>


#include "../common_src/snapshot.h"
#include "lista_queues.h"
#include "../common_src/queue.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"

#define SLEEP 200

class Processor: public Thread {
private:
    Queue<Snapshot>& queue;
    bool juego_activo;  
    ListaQueues& queues_clientes;

    void chequear_mensajes();

    void broadcast(Snapshot& msg);

public:
    explicit Processor(Queue<Snapshot>& q, ListaQueues& l);

    virtual void run() override;

    void terminar_juego() { this->juego_activo = false; }

    ~Processor();
};

#endif
