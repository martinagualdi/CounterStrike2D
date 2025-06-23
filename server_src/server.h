#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <string>
#include <iostream>

#include "mensaje_dto.h"
#include "../common_src/socket.h"
#include "acceptor.h"
#include "lista_queues.h"
#include "../common_src/queue.h"
#include "partida.h"

#define EXIT_SERVER "q"

class Server {
  private:
    Acceptor aceptador;

   
    void comenzar_a_aceptar();

    void comenzar_a_procesar();

    
    static bool argumentos_validos(int argc, char *argv[]);

    void leer_entrada();

    void terminar_juego();

  public:
    explicit Server(const char *servname);

    void start();

    ~Server();
};

#endif
