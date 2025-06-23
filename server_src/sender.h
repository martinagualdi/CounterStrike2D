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

    virtual void run() override;

    void terminar_ejecucion() {
        this->is_alive = false;
    }

    ~Sender();
};

#endif
