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
            recolectar();
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
    monitor.eliminar_todas_las_partidas();
}

void Acceptor::eliminar_cliente(ClientHandler *client) {
    client->cortar_conexion();
    delete client;
}

void Acceptor::recolectar() {
    std::cout << "[Acceptor] Recolectando clientes muertos..." << std::endl;
    clients.remove_if([this](ClientHandler *c) {
        if (c->is_dead()) {
            std::cout << "[Acceptor] El cliente con id " << c->get_id() << " ha terminado su ejecucion porque esta muerto." << std::endl;
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
    std::cout << "[Acceptor] Destruyendo Acceptor..." << std::endl;
    skt.shutdown(RW_CLOSE);
    skt.close();
    std::cout << "[Acceptor] El Acceptor ha terminado su ejecucion, va a hacer join." << std::endl;
    this->join();
    std::cout << "[Acceptor] Se joineo" << std::endl;
}