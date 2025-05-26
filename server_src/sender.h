#ifndef SENDER_H
#define SENDER_H

#include <atomic>

#include "server_protocol.h"
#include "../common_src/socket.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"

class Sender : public Thread {
  private:
    ServerProtocol& protocol;
    Queue<Snapshot> &queue_enviadora;
    std::atomic<bool> &is_alive;
    int player_id;

  public:
    explicit Sender(ServerProtocol &protocol, Queue<Snapshot> &queue, std::atomic<bool> &is_alive, int player_id);

    // Hace try_pop() de la queue de mensajes. Envia esos mensajes al cliente
    virtual void run() override;

    // Asigna el atomic "is_alive" a false, terminando la ejecucion
    void terminar_ejecucion() {
        this->is_alive = false;
    }

    ~Sender();
};

#endif
