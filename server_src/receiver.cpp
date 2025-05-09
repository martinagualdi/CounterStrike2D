#include "receiver.h"

#include <syslog.h>

#include "mensaje_dto.h"

Receiver::Receiver(Socket& s, Queue<MensajeDTO>& queue):
        protocol(s), queue_juego(queue), alive(true) {}

void Receiver::run() {

    while (alive) {
        try {
            /*
             *  LOGICA DE RECEPCION POR PROTOCOLO Y PUSH A LA QUEUE
             */
        } catch (...) {
            break;
        }
    }
}

Receiver::~Receiver() { this->join(); }