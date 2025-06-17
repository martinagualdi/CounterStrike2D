#include "client_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include <netinet/in.h>

#define BYTES_JUGADORES 28
#define BYTES_BALAS 11
#define BYTES_ARMAS 11
#define RW_CLOSE 2

ProtocoloCliente::ProtocoloCliente(const char *hostname, const char *servname) :socket(Socket(hostname, servname)) {}

void ProtocoloCliente::push_back_uint16(std::vector<uint8_t>& message, uint16_t value) {
    value = htons(value);
    message.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    message.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
}

void ProtocoloCliente::serializar_comando(ComandoDTO& comando, std::vector<uint8_t>& mensaje) {

   if(comando.tipo == DESCONECTAR){
      mensaje.push_back(PREFIJO_DESCONECTAR);
   }
   else if(comando.tipo == MOVIMIENTO){
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
   else if (comando.tipo == CAMBIAR_ARMA){
      mensaje.push_back(PREFIJO_CAMBIO_ARMA);
   }
   else if (comando.tipo == COMPRAR){
      mensaje.push_back(PREFIJO_COMPRAR);
      mensaje.push_back(static_cast<uint8_t>(comando.compra));
   }
   else if(comando.tipo == SELECCIONAR_SKIN){
      mensaje.push_back(PREFIJO_SELECCIONAR_SKIN);
      mensaje.push_back(static_cast<uint8_t>(comando.skin));
   } 
   else if(comando.tipo == ACCION_SOBRE_BOMBA){
      mensaje.push_back(PREFIJO_BOMBA);
      mensaje.push_back(static_cast<uint8_t>(comando.estado_bomba));
   } 
   else if(comando.tipo == DROPEAR){
      mensaje.push_back(PREFIJO_DROPEAR);
   } 
   else if(comando.tipo == LEVANTAR){
      mensaje.push_back(PREFIJO_LEVANTAR);
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
   size_t num_jugadores = largo_jugadores / BYTES_JUGADORES; 
   Snapshot snapshot;
   while (num_jugadores > 0) {
      uint8_t buffer[BYTES_JUGADORES];
      socket.recvall(buffer, BYTES_JUGADORES);
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
      info_jugador.esta_disparando = (buffer[19] == 0x01);
      info_jugador.esta_plantando_bomba = (buffer[20] == 0x01);
      info_jugador.esta_desactivando_bomba = (buffer[21]== 0x01);
      info_jugador.puede_comprar_ya = (buffer[22] == 0x01); // Enviar si el jugador puede comprar ya
      info_jugador.acaba_de_comprar_arma = (buffer[23] == 0x01); // Enviar si el jugador acaba de comprar arma
      info_jugador.acaba_de_comprar_balas = (buffer[24] == 0x01); // Enviar si el jugador acaba de comprar balas
      info_jugador.balas = static_cast<int>(buffer[25]); // Enviar la cantidad de balas del jugador
      info_jugador.eliminaciones_esta_ronda = static_cast<int>(buffer[26]); // Enviar las eliminaciones de esta ronda
      info_jugador.eliminaciones_totales = static_cast<int>(buffer[27]); // Enviar las eliminaciones totales del jugador
      snapshot.info_jugadores.push_back(info_jugador);
      num_jugadores--;
   }
   uint16_t largo_balas;
   socket.recvall(&largo_balas, sizeof(largo_balas));
   largo_balas = ntohs(largo_balas);
   size_t num_balas = largo_balas / BYTES_BALAS; // Cada bala ocupa 11 bytes
   while (num_balas > 0) {
      uint8_t buffer[BYTES_BALAS];
      socket.recvall(buffer, BYTES_BALAS);
      InfoMunicion info_municion;
      info_municion.id_quien_disparo = static_cast<int>(buffer[0]);
      info_municion.pos_x = static_cast<float>(ntohl(*(uint32_t*)&buffer[1])) / 100.0f;
      info_municion.pos_y = static_cast<float>(ntohl(*(uint32_t*)&buffer[5])) / 100.0f;
      info_municion.angulo_disparo = static_cast<float>(ntohs(*(uint16_t*)&buffer[9])) / 100.0f;
      snapshot.balas_disparadas.push_back(info_municion);
      num_balas--;
   }
   uint16_t largo_armas;
   socket.recvall(&largo_armas, sizeof(largo_armas));
   largo_armas = ntohs(largo_armas);
   size_t num_armas = largo_armas / BYTES_ARMAS; // Cada arma ocupa 11 bytes
   while (num_armas > 0) {
      uint8_t buffer[BYTES_ARMAS];
      socket.recvall(buffer, BYTES_ARMAS);
      InfoArmaEnSuelo info_arma;
      info_arma.tipo_arma = static_cast<enum ArmaEnMano>(buffer[0]);
      info_arma.pos_x = static_cast<float>(ntohl(*(uint32_t*)&buffer[1])) / 100.0f;
      info_arma.pos_y = static_cast<float>(ntohl(*(uint32_t*)&buffer[5])) / 100.0f;
      info_arma.municiones = static_cast<int>(ntohs(*(uint16_t*)&buffer[9]));
      snapshot.armas_sueltas.push_back(info_arma);
      num_armas--;
   }
   // Recibir la bomba
   uint32_t bomba_pos_x, bomba_pos_y;
   socket.recvall(&bomba_pos_x, sizeof(bomba_pos_x));
   socket.recvall(&bomba_pos_y, sizeof(bomba_pos_y));
   bomba_pos_x = ntohl(bomba_pos_x);
   bomba_pos_y = ntohl(bomba_pos_y);

   uint8_t esta_plantada, esta_detonada;
   socket.recvall(&esta_plantada, sizeof(esta_plantada));
   socket.recvall(&esta_detonada, sizeof(esta_detonada));

   uint16_t tiempo_para_detonar;
   socket.recvall(&tiempo_para_detonar, sizeof(tiempo_para_detonar));
   tiempo_para_detonar = ntohs(tiempo_para_detonar);

   uint8_t acaba_de_detonar, acaba_de_ser_plantada, acaba_de_ser_desactivada;
   socket.recvall(&acaba_de_detonar, sizeof(acaba_de_detonar));
   socket.recvall(&acaba_de_ser_plantada, sizeof(acaba_de_ser_plantada));
   socket.recvall(&acaba_de_ser_desactivada, sizeof(acaba_de_ser_desactivada));

   snapshot.bomba_en_suelo.pos_x = static_cast<float>(bomba_pos_x) / 100.0f;
   snapshot.bomba_en_suelo.pos_y = static_cast<float>(bomba_pos_y) / 100.0f;
   snapshot.bomba_en_suelo.estado_bomba = static_cast<enum EstadoBombaRonda>(esta_plantada);
   snapshot.bomba_en_suelo.tiempo_para_detonar = static_cast<int>(tiempo_para_detonar);
   snapshot.bomba_en_suelo.acaba_de_detonar = (acaba_de_detonar == 0x01);
   snapshot.bomba_en_suelo.acaba_de_ser_plantada = (acaba_de_ser_plantada == 0x01);
   snapshot.bomba_en_suelo.acaba_de_ser_desactivada = (acaba_de_ser_desactivada == 0x01);

   uint16_t tiempo_restante;
   socket.recvall(&tiempo_restante, sizeof(tiempo_restante));
   snapshot.tiempo_restante = static_cast<int>(ntohs(tiempo_restante));
   uint8_t equipo_ganador;
   socket.recvall(&equipo_ganador, sizeof(equipo_ganador));
   snapshot.equipo_ganador = static_cast<enum Equipo>(equipo_ganador); 
   return snapshot;
}

int ProtocoloCliente::recibirID() {
   uint16_t id_jugador;
   socket.recvall(&id_jugador, sizeof(id_jugador));
   id_jugador = ntohs(id_jugador);
   return id_jugador;
}

void ProtocoloCliente::enviar_crear_partida(std::string username) {
   uint8_t comando = PREFIJO_CREAR_PARTIDA;
   uint16_t largo = static_cast<uint16_t>(username.size());
   std::vector<uint8_t> buffer;
   buffer.push_back(comando);
   push_back_uint16(buffer, largo);
   buffer.insert(buffer.end(), username.begin(), username.end());
   if (!socket.sendall(buffer.data(), buffer.size())) {
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
   uint32_t length;
   socket.recvall(&length, sizeof(length));
   length = ntohl(length);

   std::vector<uint8_t> buffer(length);
   if(!socket.recvall(buffer.data(), length)){
      throw std::runtime_error("Error al recibir el mapa.");
   }
   std::string yaml_serializado(buffer.begin(), buffer.end());

   return yaml_serializado;
}

std::vector<std::pair<std::string, std::string>> ProtocoloCliente::recibir_lista_mapas() {
    uint8_t cantidad = 0;
    socket.recvall(&cantidad, sizeof(cantidad));

    std::vector<std::pair<std::string, std::string>> resultado;
    for (uint8_t i = 0; i < cantidad; ++i) {
        // Recibir nombre del mapa
        uint16_t len_nombre = 0;
        socket.recvall(&len_nombre, sizeof(len_nombre));
        len_nombre = ntohs(len_nombre);

        std::string nombre_mapa(len_nombre, '\0');
        socket.recvall((uint8_t*)nombre_mapa.data(), len_nombre);

        // Recibir nombre de la imagen
        uint16_t len_img = 0;
        socket.recvall(&len_img, sizeof(len_img));
        len_img = ntohs(len_img);

        std::string nombre_img(len_img, '\0');
        socket.recvall((uint8_t*)nombre_img.data(), len_img);

        resultado.emplace_back(nombre_mapa, nombre_img);
    }

    return resultado;
}

ProtocoloCliente::~ProtocoloCliente(){
   socket.shutdown(RW_CLOSE);
   socket.close();
}
