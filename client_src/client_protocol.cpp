#include "client_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include <netinet/in.h>

#define RW_CLOSE 2

ProtocoloCliente::ProtocoloCliente(const char *hostname, const char *servname) :socket(Socket(hostname, servname)) {}


void ProtocoloCliente::push_back_uint16(std::vector<uint8_t>& message, uint16_t value) {
    value = htons(value);
    message.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    message.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
}

void ProtocoloCliente::serializar_comando(ComandoDTO& comando, std::vector<uint8_t>& mensaje) {
   
   if(comando.tipo == MOVIMIENTO){
      mensaje.push_back(PREFIJO_MOVIMIENTO);
      mensaje.push_back(static_cast<uint8_t>(comando.movimiento));
   } 
   else if(comando.tipo == ROTACION){
      mensaje.push_back(PREFIJO_ROTACION);
      push_back_uint16(mensaje, comando.mouseX);
      push_back_uint16(mensaje, comando.mouseY);
   } 
   else if(comando.tipo == DISPARO){
      mensaje.push_back(PREFIJO_DISPARAR);
      //.... 
   }


}

void ProtocoloCliente::enviarComando(ComandoDTO comando) {

   std::vector<uint8_t> mensaje;

   serializar_comando(comando, mensaje);

   if (!socket.sendall(mensaje.data(), mensaje.size())) {
      throw std::runtime_error("Error al enviar el comando");
   }
}

Snapshot ProtocoloCliente::recibirSnapshot() {
   uint16_t largo;
   socket.recvall(&largo, sizeof(largo));
   largo = ntohs(largo);
   size_t num_jugadores = largo / 10;
   std::vector<Jugador> jugadores;
   while (num_jugadores > 0) {
      std::vector<uint8_t> buffer(10);
      socket.recvall(buffer.data(), 10);
      uint16_t id = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
      uint16_t pos_X = (static_cast<uint16_t>(buffer[2]) << 8) | buffer[3];
      uint16_t pos_Y = (static_cast<uint16_t>(buffer[4]) << 8) | buffer[5];
      uint32_t angulo = (static_cast<uint32_t>(buffer[6]) << 24) | buffer[7] << 16 | buffer[8] << 8  | buffer[9];
      Jugador jugador(static_cast<int>(id), static_cast<int>(pos_X), static_cast<int>(pos_Y), static_cast<float>(angulo/100));
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
