#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "receiver.h"
#include "sender.h"
#include "monitor_partidas.h"

#define RW_CLOSE 2

class ClientHandler {
private:
    Socket skt;
    ServerProtocol protocolo;
    Queue<Snapshot> queue_enviadora;
    MonitorPartidas& monitor_partidas;
    std::atomic<bool> is_alive;
    Receiver r;
    int id_client;

  public:
    explicit ClientHandler(Socket skt, MonitorPartidas& monitor_partidas, int id);

    void iniciar();

    bool is_dead() {
        return !is_alive;
    }

    void cortar_conexion();

    Queue<Snapshot> &get_queue() {
        return this->queue_enviadora;
    }

    int get_id() {
        return this->id_client;
    }

    ~ClientHandler();
};

#endif