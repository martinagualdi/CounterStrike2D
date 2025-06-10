#include "receiver.h"

#include <syslog.h>

#include "mensaje_dto.h"

Receiver::Receiver(ServerProtocol &protocolo, MonitorPartidas& monitor_partidas, std::atomic<bool> &is_alive, int player_id, Queue<Snapshot>& queue_enviadora)
    : protocol(protocolo), monitor_partidas(monitor_partidas), alive(is_alive), player_id(player_id), queue_enviadora(queue_enviadora) {}

void Receiver::asignar_queue_comandos(Queue<ComandoDTO>* q) {
    this->queue_comandos = q;
}

void Receiver::comunicacion_de_partida() {
    while (alive) {
        try {
            ComandoDTO comando;
            comando.id_jugador = player_id;
            protocol.recibir_de_cliente(comando);
            queue_comandos->try_push(comando);
        } catch (...) {
            break;
        }
    }
}

void Receiver::run() {
    comunicacion_de_partida();
}

Receiver::~Receiver() {
    this->join();
}
