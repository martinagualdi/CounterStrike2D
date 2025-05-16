#include "processor.h"

#include <vector>

#include <syslog.h>

Processor::Processor(Queue<Snapshot>& q, ListaQueues& l):
        queue(q), juego_activo(true), queues_clientes(l) {
}

void Processor::broadcast(Snapshot& msg) {
    msg.informacion = msg.informacion + " (fue reenviado por el servidor)";
    queues_clientes.broadcast(msg);
}

void Processor::chequear_mensajes() {
    Snapshot msg;
    while (queue.try_pop(msg)) {
        std::cout << "Muestro la informacion del mensaje: \n" << msg.informacion << "\n" << std::endl;
        broadcast(msg);
    }
}

void Processor::run() {
    while (juego_activo) {
        chequear_mensajes();
        /*
        LA LOGICA PARA NO QUEMAR EL CPU.
        ESTO DESPUES LO MODIFICAMOS Y HACEMOS EL LOOP DE LOS FRAMES DEL JUEGO
        */
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP));
    }
}

Processor::~Processor() { this->join(); }