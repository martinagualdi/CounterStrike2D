#include "monitor_partidas.h"

int MonitorPartidas::crear_partida(int player_id, std::string creador_username, Queue<Snapshot>& queue_enviadora, std::string yaml_path) {
    std::lock_guard<std::mutex> lock(mtx);
    partidas[id_para_partidas] = std::make_unique<Partida>(creador_username, id_para_partidas, yaml_path);
    partidas[id_para_partidas]->agregar_jugador(player_id, creador_username, queue_enviadora);
    id_para_partidas++;
    return id_para_partidas - 1;
}

bool MonitorPartidas::unirse_a_partida(int id_partida, int player_id, std::string& nombre, Queue<Snapshot>& queue_enviadora) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = partidas.find(id_partida);
    if (it != partidas.end()) 
        return it->second->agregar_jugador(player_id, nombre, queue_enviadora);
    return false;
}

std::vector<std::string> MonitorPartidas::listar_partidas() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> lista_partidas;
    for (const auto& partida : partidas) {
        std::string nueva_partida = "Id: " + std::to_string(partida.first) + "  ||  Creada por: " + partida.second->get_creador() + "\n";
        lista_partidas.push_back(nueva_partida);
    }
    return lista_partidas;
}

std::string MonitorPartidas::obtener_mapa_por_id(int id_partida) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = partidas.find(id_partida);
    if (it != partidas.end()) {
        return it->second->obtener_mapa();
    }
    return "";
}

Queue<ComandoDTO>* MonitorPartidas::obtener_queue_de_partida(int id_partida) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& partida : partidas) {
        if (partida.first == id_partida) {
            return &partida.second->get_queue();
        }
    }
    return nullptr;
}