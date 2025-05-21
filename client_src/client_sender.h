#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include "../common_src/comando_dto.h"
#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "client_protocol.h"

class ClientSender : public Thread {
private:
    ProtocoloCliente& protocolo;
    Queue<ComandoDTO>& cola_comandos;

public:

    explicit ClientSender(ProtocoloCliente& protocolo, Queue<ComandoDTO>& cola);
    void run() override;
    ClientSender(const ClientSender&) = delete;
    ClientSender& operator=(const ClientSender&) = delete;
    ClientSender(ClientSender&&) = default;
    ClientSender& operator=(ClientSender&&) = default;
    ~ClientSender();
}; 

#endif