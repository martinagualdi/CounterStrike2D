#include "acceptor.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"

Acceptor::Acceptor(const char *servname, std::vector<Partida*>& partidas) :
    skt(servname), 
    clients(), 
    aceptando_clientes(true), 
    queue_recibidora(50), /*  Valor basico como tope de la queue, despues lo modificamos con uno acorde al juego  */
    queues_clientes(), 
    processor(queue_recibidora, queues_clientes),
    partidas(partidas) {
        /* LA IDEA DEL PROCESADOR ES LO QUE LUEGO VA SUCEDER CON LA PARTIDA*/
        /* HAY QUE MANDAR LAS QUEUES Y SOCKETS PARA LA PARTIDA Y ALMACENAR/PROCESAR AHI */
        processor.start();
    }

void Acceptor::run() {
    int id = 0;
    Partida *partida = new Partida("partida1");
    partidas.push_back(partida);
    while (aceptando_clientes) {
        try {
            Socket peer = skt.accept();
            /*ClientHandler *client = new ClientHandler(std::move(peer), queue_recibidora, id);
            queues_clientes.agregar_queue(client->get_queue(), id);
            client->conectar_con_cliente();
            clients.push_back(client);
            id++;
            recolectar();*/
            std::cout << "Cliente conectado"
                      << "\n";
            partida->agregar_jugador(std::move(peer), id);
        } catch (const LibError &e) {
            if (!aceptando_clientes) {
                syslog(LOG_INFO, "%s%s. No hay clientes esperando a ser aceptados\n",
                       EXCEPCION_ESPERADA, e.what());
            } else {
                syslog(LOG_ERR, "%s%s\n", EXCEPCION_INESPERADA, e.what());
            }
        } catch (...) {
            syslog(LOG_ERR, "%s\n", EXCEPCION_DESCONOCIDA);
            break;
        }
    }
}

void Acceptor::eliminar_cliente(ClientHandler *client) {
    client->cortar_conexion();
    int id = client->get_id();
    queues_clientes.eliminar_queue(id);
    delete client;
}

void Acceptor::recolectar() {
    clients.remove_if([this](ClientHandler *c) {
        if (c->is_dead()) {
            eliminar_cliente(c);
            return true;
        }
        return false;
    });
}

void Acceptor::dejar_de_aceptar() {
    this->aceptando_clientes = false;
}

Acceptor::~Acceptor() {
    for (auto &c : clients) {
        eliminar_cliente(c);
    }
    clients.clear();
    queue_recibidora.close();
    skt.shutdown(RW_CLOSE);
    skt.close();
    this->join();
}
