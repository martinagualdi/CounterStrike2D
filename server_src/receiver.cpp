#include "receiver.h"

#include <syslog.h>

#include "mensaje_dto.h"

Receiver::Receiver(Socket &s, Queue<MensajeDTO> &queue)
    : protocol(s), queue_recibidora(queue), alive(true) {}

void Receiver::run() {

    while (alive) {
        try {
            /*
             *  LOGICA DE RECEPCION POR PROTOCOLO Y PUSH A LA QUEUE
             */
            
            MensajeDTO mensaje;
            protocol.recibir_de_cliente(mensaje);
            queue_recibidora.try_push(mensaje);
        } catch (...) {
            break;
        }
    }
}

Receiver::~Receiver() {
    this->join();
}
