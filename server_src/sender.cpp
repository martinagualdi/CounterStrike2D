#include "sender.h"

#include "../common_src/liberror.h"

Sender::Sender(Socket& skt, Queue<MensajeDTO>& q, std::atomic<bool>& is_alive):
        protocol(skt), queue(q), is_alive(is_alive) {}

void Sender::run() {
    while (is_alive) {
        try {
            /*
             * LOGICA DE "POP" A LA QUEUE Y ENVIO POR PROTOCOLO
             */
            MensajeDTO mensaje = queue.pop();
            protocol.enviar_a_cliente(mensaje);
        } catch (const ClosedQueue&) {
            break;
        }
    }
}

Sender::~Sender() { this->join(); }
