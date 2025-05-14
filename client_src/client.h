#ifndef CLIENT_H
#define CLIENT_H

#include "client_protocol.h"
#include "client_sender.h"
#include "client_event_handler.h"
#include "client_receiver.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"

class Client{

private:
    ProtocoloCliente protocolo;
    const char* username;
    Queue<uint8_t> cola_enviador;
    Queue<Snapshot> cola_recibidor;
    ClientSender hilo_enviador;
    ClientReceiver hilo_recibidor;


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