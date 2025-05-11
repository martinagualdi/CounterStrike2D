#include "client_protocol.h"

#define RW_CLOSE 2

ProtocoloCliente::ProtocoloCliente(const char *hostname, const char *servname) : socket(Socket(hostname, servname)) {}

void ProtocoloCliente::enviarComando(uint8_t comando)
{
}

Snapshot ProtocoloCliente::recibirSnapshot()
{
	return Snapshot();
}

ProtocoloCliente::~ProtocoloCliente(){
    socket.shutdown(RW_CLOSE);
    socket.close();
}
