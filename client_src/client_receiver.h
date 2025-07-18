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
    int client_id;
public:

    explicit ClientReceiver(ProtocoloCliente& protocolo, Queue<Snapshot>& cola);
    void run() override;
    bool esta_vivo() const { return should_keep_running(); } 
    void set_id(int id) { client_id = id; }
    ClientReceiver(const ClientReceiver&) = delete;
    ClientReceiver& operator=(const ClientReceiver&) = delete;
    ClientReceiver(ClientReceiver&&) = default;
    ClientReceiver& operator=(ClientReceiver&&) = default;
    ~ClientReceiver();
}; 

#endif