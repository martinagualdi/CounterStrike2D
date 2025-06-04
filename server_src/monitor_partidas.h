#ifndef MONITOR_PARTIDAS_H
#define MONITOR_PARTIDAS_H

#include <map>

#include "partida.h"


class MonitorPartidas {
private:
    std::mutex mtx;
    std::map<int, std::unique_ptr<Partida>> partidas;
    int id_para_partidas = 0;

public:
    int crear_partida(int player_id, Queue<Snapshot>& queue_enviadora, std::string yaml_path);

    bool unirse_a_partida(int id_partida_str, int player_id, Queue<Snapshot>& queue_enviadora);
    
    std::vector<std::string> listar_partidas();

    Queue<ComandoDTO>* obtener_queue_de_partida(int id_partida);



};

#endif