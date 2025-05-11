#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "client_protocol.h"

class ClientSender : public Thread {
private:
    ProtocoloCliente& protocolo;
    Queue<uint8_t>& cola_comandos;

public:

    explicit ClientSender(ProtocoloCliente& protocolo, Queue<uint8_t>& cola);
    void run() override;
    bool sigue_vivo();
    ClientSender(const ClientSender&) = delete;
    ClientSender& operator=(const ClientSender&) = delete;
    ClientSender(ClientSender&&) = default;
    ClientSender& operator=(ClientSender&&) = default;
    ~ClientSender();
}; 

#endif