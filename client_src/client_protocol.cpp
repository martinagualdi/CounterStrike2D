#include "client_protocol.h"
#include <netinet/in.h>

#define RW_CLOSE 2

ProtocoloCliente::ProtocoloCliente(const char *hostname, const char *servname) :socket(Socket(hostname, servname)) {}

void ProtocoloCliente::enviarComando(int id_jugador, uint8_t comando) {
   uint16_t id_net = htons(static_cast<uint16_t>(id_jugador));
   if (!socket.sendall(&id_net, sizeof(id_net))) {
      throw std::runtime_error("Error al enviar el id del jugador");
   }
   uint16_t comando_net = htons(static_cast<uint16_t>(comando));
   if (!socket.sendall(&comando_net, sizeof(comando_net))) {
      throw std::runtime_error("Error al enviar el comando");
   }
}

Snapshot ProtocoloCliente::recibirSnapshot() {
   
   uint8_t largo_buf[2];
   if (!socket.recvall(largo_buf, 2)) {
      throw std::runtime_error("Error al recibir el tamaño del snapshot");
   }
   uint16_t largo = (static_cast<uint16_t>(largo_buf[0]) << 8) | largo_buf[1];
   largo = ntohs(largo);

   
   size_t num_jugadores = largo / 2;
   size_t jugador_packet_size = 2 + 2 + 1;
   size_t total_bytes = num_jugadores * jugador_packet_size;

   std::vector<uint8_t> buffer(total_bytes);
   if (!socket.recvall(buffer.data(), total_bytes)) {
      throw std::runtime_error("Error al recibir los datos de los jugadores");
   }

   Snapshot snapshot;
   size_t offset = 0;
   for (size_t i = 0; i < num_jugadores; ++i) {
      if (offset + 5 > buffer.size()) break;
      uint16_t pos_X = (static_cast<uint16_t>(buffer[offset]) << 8) | buffer[offset + 1];
      uint16_t pos_Y = (static_cast<uint16_t>(buffer[offset + 2]) << 8) | buffer[offset + 3];
      offset += 5;
      Jugador jugador;
      jugador.setX(ntohs(pos_X));
      jugador.setY(ntohs(pos_Y));
      snapshot.jugadores.push_back(jugador);
   }
   return snapshot;
}

void ProtocoloCliente::recibirID(int &id_jugador) {
   uint8_t id_buf[2];
   if (!socket.recvall(id_buf, 2)) {
      throw std::runtime_error("Error al recibir el id del jugador");
   }
   uint16_t id = (static_cast<uint16_t>(id_buf[0]) << 8) | id_buf[1];
   id_jugador = ntohs(id);
}  

ProtocoloCliente::~ProtocoloCliente(){
  socket.shutdown(RW_CLOSE);
   socket.close();
}
