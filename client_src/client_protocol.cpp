#include "client_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include "../common_src/municion.h"
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
   }else if (comando.tipo == CAMBIAR_ARMA){
      mensaje.push_back(PREFIJO_CAMBIO_ARMA);
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
   uint16_t largo_jugadores;
   socket.recvall(&largo_jugadores, sizeof(largo_jugadores));
   largo_jugadores = ntohs(largo_jugadores);
   size_t num_jugadores = largo_jugadores / 19; // Cada jugador ocupa 17 bytes
   Snapshot snapshot;
   while (num_jugadores > 0) {
      uint8_t buffer[19];
      socket.recvall(buffer, 19);
      InfoJugador info_jugador;
      info_jugador.id = static_cast<int>(buffer[0]);
      info_jugador.pos_x = static_cast<float>(ntohl(*(uint32_t*)&buffer[1])) / 100.0f;
      info_jugador.pos_y = static_cast<float>(ntohl(*(uint32_t*)&buffer[5])) / 100.0f;
      info_jugador.angulo = static_cast<float>(ntohs(*(uint16_t*)&buffer[9])) / 100.0f;
      info_jugador.vida = static_cast<int>(buffer[11]);
      info_jugador.dinero = static_cast<int>(ntohs(*(uint16_t*)&buffer[12]));
      info_jugador.arma_en_mano = static_cast<enum ArmaEnMano>(buffer[14]);
      info_jugador.equipo = static_cast<enum Equipo>(buffer[15]); // Enviar el equipo del jugador
      info_jugador.skin_tipo = static_cast<enum SkinTipos>(buffer[16]); // Enviar el skin del jugador
      info_jugador.esta_vivo = (buffer[17] == 0x01);
      info_jugador.esta_moviendose = (buffer[18] == 0x01);
      snapshot.info_jugadores.push_back(info_jugador);
      num_jugadores--;
   }
   uint16_t largo_balas;
   socket.recvall(&largo_balas, sizeof(largo_balas));
   largo_balas = ntohs(largo_balas);
   size_t num_balas = largo_balas / 11; // Cada bala ocupa 11 bytes
   while (num_balas > 0) {
      uint8_t buffer[11];
      socket.recvall(buffer, 11);
      InfoMunicion info_municion;
      info_municion.id_quien_disparo = static_cast<int>(buffer[0]);
      info_municion.pos_x = static_cast<float>(ntohl(*(uint32_t*)&buffer[1])) / 100.0f;
      info_municion.pos_y = static_cast<float>(ntohl(*(uint32_t*)&buffer[5])) / 100.0f;
      info_municion.angulo_disparo = static_cast<float>(ntohs(*(uint16_t*)&buffer[9])) / 100.0f;
      snapshot.balas_disparadas.push_back(info_municion);
      num_balas--;
   }
   return snapshot;
}

int ProtocoloCliente::recibirID() {
   uint16_t id_jugador;
   socket.recvall(&id_jugador, sizeof(id_jugador));
   id_jugador = ntohs(id_jugador);
   return id_jugador;
}

void ProtocoloCliente::enviar_crear_partida() {
   uint8_t comando = PREFIJO_CREAR_PARTIDA;
   if (!socket.sendall(&comando, sizeof(comando))) {
      throw std::runtime_error("Error al enviar el comando de crear partida");
   }
}

void ProtocoloCliente::enviar_unirse_partida(int id_partida) {
   std::vector<uint8_t> buffer; 
   uint8_t comando = PREFIJO_UNIRSE_PARTIDA;
   buffer.push_back(comando);
   push_back_uint16(buffer, (uint16_t)id_partida);
   if (!socket.sendall(buffer.data(), buffer.size())) {
      throw std::runtime_error("Error al enviar el comando de unirse a partida");
   }
}

void ProtocoloCliente::enviar_listar_partida() {
   uint8_t comando = PREFIJO_LISTAR;
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

void ProtocoloCliente::enviar_mensaje(const std::string& mensaje) {
   std::vector<uint8_t> buffer;
   uint16_t largo = htons(static_cast<uint16_t>(mensaje.size()));
   buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
   buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
   buffer.insert(buffer.end(), mensaje.begin(), mensaje.end());
   if (!socket.sendall(buffer.data(), buffer.size())) {
      throw std::runtime_error("Error al enviar el mensaje");
   }
}

std::string ProtocoloCliente::recibir_mapa() {
   uint8_t header[4];
   socket.recvall(header, 4);
   uint32_t tam = (header[0] << 24) | (header[1] << 16) | (header[2] << 8) | header[3];

   std::vector<uint8_t> buffer(tam);
   socket.recvall(buffer.data(), tam);
   std::string yaml_serializado(buffer.begin(), buffer.end());

   return yaml_serializado;
}

ProtocoloCliente::~ProtocoloCliente(){
   socket.shutdown(RW_CLOSE);
   socket.close();
}
