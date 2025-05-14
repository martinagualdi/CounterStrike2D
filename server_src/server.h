#ifndef SERVER_H
#define SERVER_H

#include "mensaje_dto.h"
#include "../common_src/socket.h"
#include "processor.h"
#include "acceptor.h"
#include "lista_queues.h"
#include "../common_src/queue.h"

#define EXIT_SERVER "q"

class Server {
  private:
    Acceptor aceptador;

    // Lanza el thread aceptador
    void comenzar_a_aceptar();

    void comenzar_a_procesar();

    // Chequea que los argumentos de la funcion main sean correctos
    static bool argumentos_validos(int argc, char *argv[]);

    // Lee la entrada standard y loopea en esa entrada hasta que se ingrese una 'q'
    void leer_entrada();

    // Termina el hilo procesador y aceptador y cierra el socket
    void terminar_juego();

  public:
    explicit Server(const char *servname);

    // Inicia la logica del server. Inicializa el aceptador, procesador y las queues.
    void start();

    ~Server();
};

#endif
