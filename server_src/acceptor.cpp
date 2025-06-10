#include "acceptor.h"

#include <utility>

#include <syslog.h>

#include "../common_src/liberror.h"
#include "monitor_partidas.h"

Acceptor::Acceptor(const char *servname) :
    skt(servname), 
    clients(), 
    aceptando_clientes(true){}

void Acceptor::run() {
    int id = 0;
    MonitorPartidas monitor;
    while (aceptando_clientes) {
        try {

            Socket peer = skt.accept();
            ClientHandler *client = new ClientHandler(std::move(peer), monitor, id);
            client->iniciar();
            clients.push_back(client);
            id++;
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
    skt.shutdown(RW_CLOSE);
    skt.close();
    this->join();
}