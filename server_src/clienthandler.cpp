#include "clienthandler.h"

#include <utility>
#include <filesystem>
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

std::vector<std::pair<std::string, std::string>> ClientHandler::listar_mapas_disponibles() {
    namespace fs = std::filesystem;
    std::vector<std::pair<std::string, std::string>> mapas;

    const std::string ruta = "server_src/mapas_disponibles";

    for (const auto& entry : fs::directory_iterator(ruta)) {
        if (!entry.is_regular_file()) continue;

        auto path = entry.path();
        if (path.extension() != ".yaml") continue;

        std::string base_name = path.stem().string();
        std::string yaml_file = base_name + ".yaml";
        std::string jpg_file =  base_name + ".jpg";

        fs::path yaml_path = path.parent_path() / yaml_file;
        fs::path jpg_path = path.parent_path() / jpg_file;

        if (fs::exists(jpg_path)) {
            mapas.emplace_back(yaml_path.filename().string(), jpg_path.filename().string());
        }
    }

    return mapas;
}

void ClientHandler::comunicacion_del_lobby() {
    int partida_id;
    while (is_alive) {
        try {
            std::vector<std::string> comando_inicial = protocolo.recibir_inicio_juego();
            if (comando_inicial[0] == "crear") {
                std::vector<std::pair<std::string, std::string>>  mapas_disponibles = listar_mapas_disponibles();
                protocolo.enviar_lista_mapas(mapas_disponibles);
                std::string path = protocolo.recibir_path_mapa(); 
                path = "server_src/mapas_disponibles/" + path;
                partida_id = monitor_partidas.crear_partida(id_client, comando_inicial[1], queue_enviadora, path);
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                protocolo.enviar_mapa(yaml_serializado);
                break;
            } else if (comando_inicial[0] == "unirse") {
                if (!monitor_partidas.unirse_a_partida(std::stoi(comando_inicial[1]), id_client, comando_inicial[2], queue_enviadora)) {
                    continue;
                }
                partida_id = std::stoi(comando_inicial[1]);
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                protocolo.enviar_mapa(yaml_serializado);
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
