#include "clienthandler.h"
#include "../common_src/ruta_base.h"
#include "../common_src/liberror.h"
#include <utility>
#include <filesystem>
#include <syslog.h>

ClientHandler::ClientHandler(Socket s, MonitorPartidas& monitor_partidas, int id): 
    skt(std::move(s)),
    protocolo(skt), 
    queue_enviadora(50), 
    monitor_partidas(monitor_partidas),
    is_alive(true), 
    r(protocolo, monitor_partidas, is_alive, id, queue_enviadora),
    id_client(id) {}

void ClientHandler::iniciar() {
    r.start();
}

void ClientHandler::cortar_conexion() {
    is_alive = false;
}

ClientHandler::~ClientHandler() {
    r.terminar_ejecucion();
    r.join();
}
