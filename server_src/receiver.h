#ifndef RECEIVER_H
#define RECEIVER_H

#include <atomic>

#include "mensaje_dto.h"
#include "server_protocol.h"
#include "../common_src/socket.h"

#include "queue.h"
#include "thread.h"

#define EXCEPCION_CONOCIDA "Se produjo una excepcion conocida: "
#define EXCEPCION_INESPERADA "Se produjo una excepcion inesperada: "
#define EXCEPCION_DESCONOCIDA "Se produjo una exepcion desconocida"

/*
HAY QUE HACER USO DEL PROTOCOLO, NO LO IMPLEMENTE TODAVIA
*/

class Receiver: public Thread {
private:
    ServerProtocol protocol;
    Queue<MensajeDTO>& queue_juego;
    std::atomic<bool> alive;

public:
    explicit Receiver(Socket& skt, Queue<MensajeDTO>& queue_juego);

    // Recibe el mensaje del cliente y lo agrega a la queue de mensajes del procesador
    virtual void run() override;

    // Asigna el valor del atomic "alive" a false, terminando la ejecucion
    void terminar_ejecucion() { this->alive = false; }

    virtual ~Receiver();
};

#endif