#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <atomic>
#include <list>

#include "../common_src/snapshot.h"
#include "../common_src/socket.h"
#include "processor.h"
#include "clienthandler.h"
#include "lista_queues.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"

#define RW_CLOSE 2
#define EXCEPCION_ESPERADA "Excepcion esperada: "

class Acceptor : public Thread {
  private:
    Socket skt;
    std::list<ClientHandler *> clients;
    std::atomic<bool> aceptando_clientes;
    Queue<Snapshot> queue_recibidora;
    ListaQueues queues_clientes;
    Processor processor;

    // Recorre la lista de clientes y elimina a los clientes que ya no se encuentran activos,
    // asegurando que no hay memoria ocupada por threads que ya terminaron
    void recolectar();

    // Corta la conexion del thread con el cliente verdadero, elimina su queue de mensajes y luego
    // elimina al thread
    void eliminar_cliente(ClientHandler *client);

  public:
    explicit Acceptor(const char *servname);

    // Realiza el loop aceptando nuevos clientes y lanzando nuevos hilos para manejar a cada nuevo
    // cliente. En cada iteracion se encarga de eliminar a los clientes que ya hayan terminado
    virtual void run() override;

    // Cambia el atomics 'aceptando_clientes' a false para no aceptar nuevos clientes
    void dejar_de_aceptar();

    virtual ~Acceptor();
};

#endif
