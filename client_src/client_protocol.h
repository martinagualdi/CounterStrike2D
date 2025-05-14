#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common_src/snapshot.h"
#include "../common_src/queue.h"
#include "../common_src/thread.h"
#include "../common_src/socket.h"

class ProtocoloCliente {
private:
  //  Socket socket;

public:

    explicit ProtocoloCliente(const char* hostname, const char* servname);
    
    void enviarComando(uint8_t comando);
    Snapshot recibirSnapshot();

    ProtocoloCliente(const ProtocoloCliente&) = delete;
    ProtocoloCliente& operator=(const ProtocoloCliente&) = delete;
    ProtocoloCliente(ProtocoloCliente&&) = delete;
    ProtocoloCliente& operator=(ProtocoloCliente&&) = delete;
    ~ProtocoloCliente();    
};

#endif 