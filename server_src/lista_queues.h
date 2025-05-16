#ifndef LISTA_QUEUES_H
#define LISTA_QUEUES_H

#include <list>
#include <mutex>
#include <string>
#include <utility>

#include "../common_src/snapshot.h"
#include "../common_src/queue.h"

class ListaQueues {
  private:
    std::mutex mtx;
    std::list<std::pair<Queue<Snapshot> &, int>> lista_queues;

  public:
    ListaQueues();

    // Recibe un mensaje y lo agrega a todas las queues de clientes que tiene en su lista, a menos
    // que esten cerradas
    void broadcast(const Snapshot &msg);

    int get_size() { return lista_queues.size(); }

    // Agrega una nueva queue a la lista de queues, con su id correspondiente
    void agregar_queue(Queue<Snapshot> &q, int id_client);

    // Elimina la queue con el id recibido de la lista
    void eliminar_queue(int id_client);
};

#endif
