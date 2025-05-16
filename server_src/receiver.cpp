#include "receiver.h"

#include <syslog.h>

#include "mensaje_dto.h"

Receiver::Receiver(Socket &skt, Queue<ComandoDTO> &queue, std::atomic<bool> &is_alive, int player_id)
    : protocol(skt), queue_comandos(queue), alive(is_alive), player_id(player_id) {}

void Receiver::run() {

    while (alive) {
        try {
            /*
             *  LOGICA DE RECEPCION POR PROTOCOLO Y PUSH A LA QUEUE
             */
            
            ComandoDTO comando;
            protocol.recibir_de_cliente(comando);
            queue_comandos.try_push(comando);
        } catch (...) {
            break;
        }
    }
}

Receiver::~Receiver() {
    this->join();
}
