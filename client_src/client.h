#ifndef CLIENT_H
#define CLIENT_H

#include "client_protocol.h"
#include "client_sender.h"
#include "client_receiver.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"
#include "../common_src/configuracion.h"

class Client{

private:
    ProtocoloCliente protocolo;
    const char* username;
    int cliente_id;
    bool clienteActivo;
    Queue<ComandoDTO> cola_enviador;
    Queue<Snapshot> cola_recibidor;
    ClientSender hilo_enviador;
    ClientReceiver hilo_recibidor;
    bool puede_comprar;
    bool hilos_iniciados = false;
    
public:

    explicit Client(const char* hostname, const char* servname, const char* username);
    void iniciar();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) = default;
    Client& operator=(Client&&) = default;
    ~Client();
};


#endif