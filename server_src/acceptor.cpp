#include "acceptor.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"


Acceptor::Acceptor(const char* servname, Queue<MensajeDTO>& q, ListaQueues& l):
        skt(servname), aceptando_clientes(true), queue_juego(q), queues_clientes(l) {}

void Acceptor::run() {
    int id = 0;
    while (aceptando_clientes) {
        try {
            Socket peer = skt.accept();
            ClientHandler* client = new ClientHandler(std::move(peer), queue_juego, id);
            queues_clientes.agregar_queue(client->get_queue(), id);
            client->conectar_con_cliente();
            clients.push_back(client);
            id++;
            recolectar();
        } catch (const LibError& e) {
            if (!aceptando_clientes) {
                syslog(LOG_INFO, "%s%s. No hay clientes esperando a ser aceptados\n",
                       EXCEPCION_ESPERADA, e.what());
            } else {
                syslog(LOG_ERR, "%s%s\n", EXCEPCION_INESPERADA, e.what());
            }
            break;
        } catch (...) {
            syslog(LOG_ERR, "%s\n", EXCEPCION_DESCONOCIDA);
            break;
        }
    }
}

void Acceptor::eliminar_cliente(ClientHandler* client) {
    client->cortar_conexion();
    int id = client->get_id();
    queues_clientes.eliminar_queue(id);
    delete client;
}

void Acceptor::recolectar() {
    clients.remove_if([this](ClientHandler* c) {
        if (c->is_dead()) {
            eliminar_cliente(c);
            return true;
        }
        return false;
    });
}

void Acceptor::dejar_de_aceptar() { this->aceptando_clientes = false; }

Acceptor::~Acceptor() {
    for (auto& c: clients) {
        eliminar_cliente(c);
    }
    clients.clear();
    queue_juego.close();
    skt.shutdown(RW_CLOSE);
    skt.close();
    this->join();
}