#include "sender.h"

#include "../common_src/liberror.h"

Sender::Sender(ServerProtocol &protocol, Queue<Snapshot> &q, std::atomic<bool> &is_alive, int player_id)
    : protocol(protocol), is_alive(is_alive), queue_enviadora(q), player_id(player_id) {
        protocol.enviarID(player_id);
    }

void Sender::run() {
    while (is_alive) {
        try {
            /*
             * LOGICA DE "POP" A LA QUEUE Y ENVIO POR PROTOCOLO
             */
            Snapshot snapshot;
            queue_enviadora.try_pop(snapshot); 
            protocol.enviar_a_cliente(snapshot); 
        } catch (const ClosedQueue &) {
            break;
        }
    }
}

Sender::~Sender() {
    this->join();
}
