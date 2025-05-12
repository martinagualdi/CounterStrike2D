#include "processor.h"

#include <vector>

#include <syslog.h>

#include "mensaje_dto.h"

Processor::Processor(Queue<MensajeDTO>& q, ListaQueues& l):
        queue(q), juego_activo(true), queues_clientes(l) {
}

void Processor::broadcast(MensajeDTO& msg) {
    msg.informacion = msg.informacion + " (fue reenviado por el servidor)";
    queues_clientes.broadcast(msg);
}

void Processor::chequear_mensajes() {
    MensajeDTO msg;
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