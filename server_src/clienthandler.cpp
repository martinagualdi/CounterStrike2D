#include "clienthandler.h"

#include "../common_src/liberror.h"

ClientHandler::ClientHandler(Socket s, MonitorPartidas& monitor_partidas, int id): 
    skt(std::move(s)),
    protocolo(skt), 
    queue_enviadora(50), /*  Valor basico como tope de la queue, despues lo modificamos con uno acorde al juego  */
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
    //s.terminar_ejecucion();
    r.terminar_ejecucion();
    queue_enviadora.close();
    skt.shutdown(RW_CLOSE);
    skt.close();
}
