#include "clienthandler.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"

ClientHandler::ClientHandler(Socket s, MonitorPartidas& monitor_partidas, int id): 
    skt(std::move(s)),
    protocolo(skt), 
    queue_enviadora(50), /*  Valor basico como tope de la queue, despues lo modificamos con uno acorde al juego  */
    monitor_partidas(monitor_partidas),
    is_alive(true), 
    r(protocolo, monitor_partidas, is_alive, id, queue_enviadora),
    s(protocolo, queue_enviadora, is_alive, id), 
    id_client(id) {}

void ClientHandler::comunicacion_del_lobby() {
    int partida_id;
    while (is_alive) {
        try {
            std::vector<std::string> comando_inicial = protocolo.recibir_inicio_juego();
            if (comando_inicial[0] == "crear") {
                /*
                Aca iria la logica de elegir el mapa, que se debe recibir via socket el yaml del mapa
                y se debe crear un objeto Mapa que se envie al monitor de partidas y ahi al gameloop de la partida.
                */
                std::string path = protocolo.recibir_path_mapa(); 
                partida_id = monitor_partidas.crear_partida(id_client, queue_enviadora, path);
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                protocolo.enviar_mapa(yaml_serializado);
                break;
            } else if (comando_inicial[0] == "unirse") {
                if (!monitor_partidas.unirse_a_partida(std::stoi(comando_inicial[1]), id_client, queue_enviadora)) {
                    continue;
                }
                partida_id = std::stoi(comando_inicial[1]);
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                protocolo.enviar_mapa(yaml_serializado);
                /*
                Aca iria la logica del mapa ya debe estar dentro del gameloop porque no la estas creando.
                */
                break;
            } else {
                std::vector<std::string> lista_partidas = monitor_partidas.listar_partidas();
                protocolo.enviar_lista_partidas(lista_partidas);
            }
        } catch (...) {
            break;
        }
    }
    Queue<ComandoDTO>* queue_cmds = monitor_partidas.obtener_queue_de_partida(partida_id);
    r.asignar_queue_comandos(queue_cmds);
}

void ClientHandler::iniciar() {
    comunicacion_del_lobby();

    r.start();
    s.start();
}

void ClientHandler::cortar_conexion() {
    is_alive = false;
}

ClientHandler::~ClientHandler() {
    s.terminar_ejecucion();
    r.terminar_ejecucion();
    queue_enviadora.close();
    skt.shutdown(RW_CLOSE);
    skt.close();
}
