#include "receiver.h"

#include <syslog.h>

#include "mensaje_dto.h"

Receiver::Receiver(ServerProtocol &protocolo, MonitorPartidas& monitor_partidas, std::atomic<bool> &is_alive, int player_id, Queue<Snapshot>& queue_enviadora)
    : protocol(protocolo), monitor_partidas(monitor_partidas), alive(is_alive), player_id(player_id), queue_enviadora(queue_enviadora) {}


void Receiver::comunicacion_del_lobby() {
    int partida_id;
    while (alive) {
        try {
            std::vector<std::string> comando_inicial = protocol.recibir_inicio_juego();
            if (comando_inicial[0] == "crear") {
                partida_id = monitor_partidas.crear_partida(player_id, queue_enviadora);
                break;
            } else if (comando_inicial[0] == "unirse") {
                if (!monitor_partidas.unirse_a_partida(std::stoi(comando_inicial[1]), player_id, queue_enviadora)) {
                    protocol.enviar_mensaje("Error: ID de partida no valido");
                    continue;
                }
                partida_id = std::stoi(comando_inicial[1]);
                break;
            } else {
                std::vector<std::string> lista_partidas = monitor_partidas.listar_partidas();
                protocol.enviar_lista_partidas(lista_partidas);
            }
        } catch (...) {
            break;
        }
    }
    queue_comandos = monitor_partidas.obtener_queue_de_partida(partida_id);
}

void Receiver::comunicacion_de_partida() {
    while (alive) {
        try {
            /*
             *  LOGICA DE RECEPCION POR PROTOCOLO Y PUSH A LA QUEUE
             */
            
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
    comunicacion_del_lobby();
    comunicacion_de_partida();
}

Receiver::~Receiver() {
    this->join();
}
