#include "clienthandler.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"

ClientHandler::ClientHandler(Socket &&s, Queue<MensajeDTO> &queue, int id)
    : skt(std::move(s)), queue_juego(50), is_alive(true), r(skt, queue),
      s(skt, queue_juego, is_alive), id_client(id) {}

void ClientHandler::conectar_con_cliente() {
    r.start();
    s.start();
}

void ClientHandler::cortar_conexion() {
    is_alive = false;
}

ClientHandler::~ClientHandler() {
    s.terminar_ejecucion();
    r.terminar_ejecucion();
    queue_juego.close();
    skt.shutdown(RW_CLOSE);
    skt.close();
}
