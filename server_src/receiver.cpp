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
                /*
                Aca iria la logica de elegir el mapa, que se debe recibir via socket el yaml del mapa
                y se debe crear un objeto Mapa que se envie al monitor de partidas y ahi al gameloop de la partida.
                */
                std::string path = protocol.recibir_path_mapa(); 
                partida_id = monitor_partidas.crear_partida(player_id, comando_inicial[1], queue_enviadora, path);
                protocol.enviar_mensaje(monitor_partidas.obtener_mapa_por_id(partida_id));
                break;
            } else if (comando_inicial[0] == "unirse") {
                std::cout << "Unirse a partida: " << comando_inicial[1] << std::endl;
                if (!monitor_partidas.unirse_a_partida(std::stoi(comando_inicial[1]), player_id, queue_enviadora)) {
                    continue;
                }
                partida_id = std::stoi(comando_inicial[1]);
                protocol.enviar_mensaje(monitor_partidas.obtener_mapa_por_id(partida_id));
                /*
                Aca iria la logica del mapa ya debe estar dentro del gameloop porque no la estas creando.
                */
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
