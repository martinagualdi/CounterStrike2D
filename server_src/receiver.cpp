#include "receiver.h"
#include "mensaje_dto.h"
#include "../common_src/ruta_base.h"
#include "../common_src/liberror.h"

#include <syslog.h>

#define MENSAJE_EXITO "success"
#define MENSAJE_ERROR "failed"
#define MENSAJE_CANCELAR "cancell"

Receiver::Receiver(ServerProtocol &protocolo, MonitorPartidas& monitor_partidas, std::atomic<bool> &is_alive, int player_id, Queue<Snapshot>& queue_enviadora)
    : protocol(protocolo), monitor_partidas(monitor_partidas), alive(is_alive), player_id(player_id), game_id(-1), queue_enviadora(queue_enviadora), sender(protocol, queue_enviadora, is_alive, player_id) {}

std::vector<std::pair<std::string, std::string>> Receiver::listar_mapas_disponibles() {
    namespace fs = std::filesystem;
    std::vector<std::pair<std::string, std::string>> mapas;

    const std::string ruta = RUTA_SERVER_BASE;

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

void Receiver::comunicacion_del_lobby() {
    int partida_id;
    while (alive) {
        try {
            std::vector<std::string> comando_inicial = protocol.recibir_inicio_juego();
            if (comando_inicial[0] == COMANDO_SALIR) {
                alive = false;
                return;
            }else if (comando_inicial[0] == COMANDO_CREAR) {
                std::vector<std::pair<std::string, std::string>>  mapas_disponibles = listar_mapas_disponibles();
                protocol.enviar_lista_mapas(mapas_disponibles);
                std::string path = protocol.recibir_path_mapa(); 
                if (path == MENSAJE_CANCELAR) {
                    continue;
                }
                std::string path_completo = RUTA_SERVER_BASE + path;
                partida_id = monitor_partidas.crear_partida(player_id, comando_inicial[1], queue_enviadora, path_completo);
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                game_id = partida_id;
                protocol.enviar_mapa(yaml_serializado);
                protocol.enviar_valores_de_config(InfoConfigClient(true));
                break;
            } else if (comando_inicial[0] == COMANDO_UNIRSE) {
                if (!monitor_partidas.unirse_a_partida(std::stoi(comando_inicial[1]), player_id, comando_inicial[2], queue_enviadora)) {
                    protocol.enviar_mensaje(MENSAJE_ERROR);
                    continue;
                }
                protocol.enviar_mensaje(MENSAJE_EXITO);
                partida_id = std::stoi(comando_inicial[1]);
                game_id = partida_id;
                std::string yaml_serializado = monitor_partidas.obtener_mapa_por_id(partida_id);
                protocol.enviar_mapa(yaml_serializado);
                protocol.enviar_valores_de_config(InfoConfigClient(true));
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
    sender.start();
}

void Receiver::comunicacion_de_partida() {
    bool desconectar = false;
    while (alive) {
        ComandoDTO comando;
        comando.id_jugador = player_id;
        try {            
            protocol.recibir_de_cliente(comando);
        } catch (...) {
            break;
        }
        try {
            queue_comandos->try_push(comando);
            if (desconectar)
                break;
            if (comando.tipo == DESCONECTAR) 
                desconectar = true;
        } catch (const ClosedQueue& e) {
            break;
        } catch (...) {
            break;
        }
    }
    alive = false;
}

void Receiver::run() {
    comunicacion_del_lobby();
    comunicacion_de_partida();
    if (game_id != -1) 
        monitor_partidas.eliminar_jugador_de_partida(game_id, player_id);
    queue_enviadora.close();
    if (game_id != -1) 
        sender.join();
}

Receiver::~Receiver() {}