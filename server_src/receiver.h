#ifndef RECEIVER_H
#define RECEIVER_H

#include <atomic>

#include "server_protocol.h"
#include "../common_src/snapshot.h"
#include "../common_src/socket.h"

#include "../common_src/queue.h"
#include "../common_src/thread.h"

#define EXCEPCION_CONOCIDA "Se produjo una excepcion conocida: "
#define EXCEPCION_INESPERADA "Se produjo una excepcion inesperada: "
#define EXCEPCION_DESCONOCIDA "Se produjo una exepcion desconocida"

/*
HAY QUE HACER USO DEL PROTOCOLO, NO LO IMPLEMENTE TODAVIA
*/

class Receiver : public Thread {
  private:
    ServerProtocol protocol;
    Queue<ComandoDTO> &queue_comandos;
    std::atomic<bool>& alive;
    int player_id;

  public:
    explicit Receiver(Socket &skt, Queue<ComandoDTO> &queue, std::atomic<bool> &is_alive, int player_id);

    // Recibe el mensaje del cliente y lo agrega a la queue de mensajes del procesador
    virtual void run() override;

    // Asigna el valor del atomic "alive" a false, terminando la ejecucion
    void terminar_ejecucion() {
        this->alive = false;
    }

    virtual ~Receiver();
};

#endif
