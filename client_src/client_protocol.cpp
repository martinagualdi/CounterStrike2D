#include "client_protocol.h"

#define RW_CLOSE 2


ProtocoloCliente::ProtocoloCliente(const char *hostname, const char *servname) :socket(Socket(hostname, servname)) {}

void ProtocoloCliente::enviarComando(uint8_t comando)
{
   if (!socket.sendall(&comando, sizeof(comando))){
      throw std::runtime_error("Error al enviar el comando");
   };
}

Snapshot ProtocoloCliente::recibirSnapshot()
{
   Snapshot snapshot;
   if (!socket.recvall(&snapshot, sizeof(snapshot))){
      throw std::runtime_error("Error al recibir el snapshot");
   }
   return snapshot;
}



ProtocoloCliente::~ProtocoloCliente(){
   // socket.shutdown(RW_CLOSE);
   // socket.close();
}
