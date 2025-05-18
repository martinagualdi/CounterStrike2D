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
   uint16_t largo;
   socket.recvall(&largo, sizeof(largo));
   largo = ntohs(largo);
   size_t num_jugadores = largo / 6;
   std::vector<Jugador> jugadores;
   while (num_jugadores > 0){
      std::vector<uint8_t> buffer(6);
      socket.recvall(buffer.data(), 6);
      uint16_t id = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
      uint16_t pos_X = (static_cast<uint16_t>(buffer[2]) << 8) | buffer[3];
      uint16_t pos_Y = (static_cast<uint16_t>(buffer[4]) << 8) | buffer[5];  
      Jugador jugador(static_cast<int>(id), static_cast<int>(pos_X), static_cast<int>(pos_Y));
      jugadores.push_back(jugador);
      num_jugadores--;
   }
   return Snapshot(jugadores);
}

int ProtocoloCliente::recibirID() {
   uint16_t id_jugador;
   socket.recvall(&id_jugador, sizeof(id_jugador));
   id_jugador = ntohs(id_jugador);
   return id_jugador;
}  

ProtocoloCliente::~ProtocoloCliente(){
  socket.shutdown(RW_CLOSE);
   socket.close();
}
