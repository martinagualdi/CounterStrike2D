#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "client_protocol.h"

class ClientReceiver : public Thread {
private:
    ProtocoloCliente& protocolo;
    Queue<Snapshot>& cola_snapshots;

public:

    explicit ClientReceiver(ProtocoloCliente& protocolo, Queue<Snapshot>& cola);
    void run() override;
    ClientReceiver(const ClientReceiver&) = delete;
    ClientReceiver& operator=(const ClientReceiver&) = delete;
    ClientReceiver(ClientReceiver&&) = default;
    ClientReceiver& operator=(ClientReceiver&&) = default;
    ~ClientReceiver();
}; 

#endif