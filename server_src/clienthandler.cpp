#include "clienthandler.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"

ClientHandler::ClientHandler(Socket s, Queue<ComandoDTO> &queue_comandos, int id)
    : skt(std::move(s)), 
    queue_enviadora(50), /*  Valor basico como tope de la queue, despues lo modificamos con uno acorde al juego  */
    is_alive(true), 
    r(skt, queue_comandos, is_alive, id),
    s(skt, queue_enviadora, is_alive, id), 
    id_client(id) {}

void ClientHandler::iniciar() {
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
