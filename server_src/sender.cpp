#include "sender.h"

#include "../common_src/liberror.h"

Sender::Sender(ServerProtocol &protocol, Queue<Snapshot> &q, std::atomic<bool> &is_alive, int player_id)
    : protocol(protocol), queue_enviadora(q), is_alive(is_alive), player_id(player_id) {
        protocol.enviarID(player_id);
    }

void Sender::run() {
    while (is_alive) {
        try {
            Snapshot snapshot = queue_enviadora.pop(); 
            protocol.enviar_a_cliente(snapshot); 
        } catch (const ClosedQueue &) {
            break;
        }
    }
    is_alive = false;
}

Sender::~Sender() {}
