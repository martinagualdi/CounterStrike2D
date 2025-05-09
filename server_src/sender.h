#ifndef SENDER_H
#define SENDER_H

#include <atomic>

#include "mensaje_dto.h"
#include "server_protocol.h"
#include "../common_src/socket.h"

#include "queue.h"
#include "thread.h"

class Sender: public Thread {
private:
    ServerProtocol protocol;
    Queue<MensajeDTO>& queue;
    std::atomic<bool>& is_alive;

public:
    explicit Sender(Socket& skt, Queue<MensajeDTO>& queue, std::atomic<bool>& is_alive);

    // Hace try_pop() de la queue de mensajes. Envia esos mensajes al cliente
    virtual void run() override;

    // Asigna el atomic "is_alive" a false, terminando la ejecucion
    void terminar_ejecucion() { this->is_alive = false; }

    ~Sender();
};


#endif
