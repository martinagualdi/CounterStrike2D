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
   else if(comando.tipo == DISPARO){
      mensaje.push_back(PREFIJO_DISPARAR);
      uint16_t angulo = static_cast<uint16_t>(comando.angulo * 100);      
      push_back_uint16(mensaje, angulo);
   }
   else if(comando.tipo == ROTACION){
      mensaje.push_back(PREFIJO_ROTACION);
      uint16_t angulo = static_cast<uint16_t>(comando.angulo * 100);      
      push_back_uint16(mensaje, angulo);
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
   size_t num_jugadores = largo / 12;
   std::vector<Jugador> jugadores;
   while (num_jugadores > 0) {
      uint8_t buffer[12];
      socket.recvall(buffer, 12);
      uint16_t id = ntohs(*(uint16_t*)&buffer[0]);
      uint32_t pos_X = ntohl(*(uint32_t*)&buffer[2]);
      uint32_t pos_Y = ntohl(*(uint32_t*)&buffer[6]);
      uint16_t angulo = ntohs(*(uint16_t*)&buffer[10]);
      Jugador jugador(static_cast<float>(id), static_cast<float>(pos_X)/100, static_cast<float>(pos_Y)/100, static_cast<float>(angulo)/100);
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

void ProtocoloCliente::enviar_crear_partida() {
   uint8_t comando = 0x0A;
   if (!socket.sendall(&comando, sizeof(comando))) {
      throw std::runtime_error("Error al enviar el comando de crear partida");
   }
}

void ProtocoloCliente::enviar_unirse_partida(int id_partida) {
   std::vector<uint8_t> buffer(2);
   uint8_t comando = 0x0B;
   buffer.push_back(comando);
   push_back_uint16(buffer, (uint16_t)id_partida);
   if (!socket.sendall(buffer.data(), buffer.size())) {
      throw std::runtime_error("Error al enviar el comando de unirse a partida");
   }
}

void ProtocoloCliente::enviar_listar_partida() {
   uint8_t comando = 0x0C;
   if (!socket.sendall(&comando, sizeof(comando))) {
      throw std::runtime_error("Error al enviar el comando de listar partidas");
   }
}

std::string ProtocoloCliente::recibir_lista_partidas() {
   uint16_t largo;
   socket.recvall(&largo, sizeof(largo));
   largo = ntohs(largo);
   std::vector<uint8_t> buffer(largo);
   socket.recvall(buffer.data(), largo);
   std::string lista_partidas(buffer.begin(), buffer.end());
   return lista_partidas;
}

ProtocoloCliente::~ProtocoloCliente(){
   socket.shutdown(RW_CLOSE);
   socket.close();
}
