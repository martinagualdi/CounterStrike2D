#include "server_protocol.h"
#include "../common_src/prefijos_protocolo.h"
#include <vector>
#include <iostream>
#include <arpa/inet.h>


#define BYTES_JUGADORES 28
#define BYTES_BALAS 11
#define BYTES_ARMAS 11


void ServerProtocol::push_back_uint16_t(std::vector<uint8_t> &buffer, uint16_t value) {
    value = htons(value);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
}

void ServerProtocol::push_back_uint32_t(std::vector<uint8_t> &buffer, uint32_t value) {
    value = htonl(value);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[1]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[2]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&value)[3]);
}

ServerProtocol::ServerProtocol(Socket &skt) : skt(skt) {}

bool ServerProtocol::enviar_a_cliente(const Snapshot& snapshot) {     
    if (snapshot.info_jugadores.size() <= 0) {
        return false;
    }
    std::vector<uint8_t> buffer;
    uint16_t largo_jugadores = htons(static_cast<uint16_t>(snapshot.info_jugadores.size()));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_jugadores)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_jugadores)[1]);
    for (const InfoJugador& j : snapshot.info_jugadores) {
        push_back_uint16_t(buffer, static_cast<uint16_t>(j.nombre.size()));
        buffer.insert(buffer.end(), j.nombre.begin(), j.nombre.end());
        uint8_t id = static_cast<uint8_t>(j.id);
        buffer.push_back(id);
        push_back_uint32_t(buffer, static_cast<uint32_t>(j.pos_x * 100));
        push_back_uint32_t(buffer, static_cast<uint32_t>(j.pos_y * 100));
        push_back_uint16_t(buffer, static_cast<uint16_t>(j.angulo * 100));
        buffer.push_back(static_cast<uint8_t>(j.vida));
        push_back_uint16_t(buffer, static_cast<uint16_t>(j.dinero));
        buffer.push_back(static_cast<uint8_t>(j.arma_en_mano));
        buffer.push_back(static_cast<uint8_t>(j.equipo)); // Enviar el equipo del jugador
        buffer.push_back(static_cast<uint8_t>(j.skin_tipo)); // Enviar el skin del jugador
        uint8_t esta_vivo = j.esta_vivo ? 0x01 : 0x00;
        buffer.push_back(esta_vivo); // Enviar si el jugador está vivo
        uint8_t esta_moviendose = j.esta_moviendose ? 0x01 : 0x00;
        buffer.push_back(esta_moviendose); // Enviar si el jugador está moviéndose
        uint8_t esta_disparando = j.esta_disparando ? 0x01 : 0x00;
        buffer.push_back(esta_disparando); // Enviar si el jugador está disparando
        uint8_t esta_plantando_bomba = j.esta_plantando_bomba ? 0x01 : 0x00;
        buffer.push_back(esta_plantando_bomba); // Enviar si el jugador está plantando bomba
        uint8_t puede_comprar_ya = j.puede_comprar_ya ? 0x01 : 0x00;
        buffer.push_back(puede_comprar_ya); // Enviar si el jugador puede comprar ya
        uint8_t acaba_de_comprar_arma = j.acaba_de_comprar_arma ? 0x01 : 0x00;
        buffer.push_back(acaba_de_comprar_arma); // Enviar si el jugador acaba de comprar arma
        uint8_t acaba_de_comprar_balas = j.acaba_de_comprar_balas ? 0x01 : 0x00;
        buffer.push_back(acaba_de_comprar_balas); // Enviar si el jugador acaba de comprar balas
        uint8_t balas = static_cast<uint8_t>(j.balas);
        buffer.push_back(balas); // Enviar la cantidad de balas del jugador
        uint8_t eliminaciones_esta_ronda = static_cast<uint8_t>(j.eliminaciones_esta_ronda);
        buffer.push_back(eliminaciones_esta_ronda); // Enviar las eliminaciones de esta ronda
        uint8_t eliminaciones_totales = static_cast<uint8_t>(j.eliminaciones_totales);
        buffer.push_back(eliminaciones_totales); // Enviar las eliminaciones totales del jugador
        uint8_t muertes = static_cast<uint8_t>(j.muertes);
        buffer.push_back(muertes); // Enviar las muertes del jugador
    }
    uint16_t largo_balas = htons(static_cast<uint16_t>(snapshot.balas_disparadas.size() * BYTES_BALAS));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_balas)[1]);
    for (const InfoMunicion& bala : snapshot.balas_disparadas) {
        buffer.push_back(static_cast<uint8_t>(bala.id_quien_disparo));
        push_back_uint32_t(buffer, static_cast<uint32_t>(bala.pos_x * 100));
        push_back_uint32_t(buffer, static_cast<uint32_t>(bala.pos_y * 100));
        push_back_uint16_t(buffer, static_cast<uint16_t>(bala.angulo_disparo * 100));
    }
    uint16_t largo_armas = htons(static_cast<uint16_t>(snapshot.armas_sueltas.size() * BYTES_ARMAS));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_armas)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo_armas)[1]);
    for (const InfoArmaEnSuelo& arma : snapshot.armas_sueltas) {
        buffer.push_back(static_cast<uint8_t>(arma.tipo_arma));
        push_back_uint32_t(buffer, static_cast<uint32_t>(arma.pos_x * 100));
        push_back_uint32_t(buffer, static_cast<uint32_t>(arma.pos_y * 100));
        push_back_uint16_t(buffer, static_cast<uint16_t>(arma.municiones)); 
    }
    push_back_uint16_t(buffer, static_cast<uint16_t>(snapshot.tiempo_restante)); //Enviar tiempo restante
    buffer.push_back(static_cast<uint8_t>(snapshot.rondas_info.rondas_ganadas_ct)); // Enviar rondas ganadas CT
    buffer.push_back(static_cast<uint8_t>(snapshot.rondas_info.rondas_ganadas_tt)); // Enviar rondas ganadas TT
    buffer.push_back(static_cast<uint8_t>(snapshot.rondas_info.ronda_actual)); // Enviar ronda actual
    buffer.push_back(static_cast<uint8_t>(snapshot.rondas_info.total_rondas)); // Enviar total de rondas
    buffer.push_back(static_cast<uint8_t>(snapshot.equipo_ganador)); // Enviar el equipo ganador
    skt.sendall(buffer.data(), buffer.size());
    return true;
}

bool ServerProtocol::recibir_de_cliente(ComandoDTO& comando) {

    uint8_t prefijo;
    skt.recvall(&prefijo, sizeof(prefijo));
    
    switch(prefijo){
        case PREFIJO_DESCONECTAR:
            comando.tipo = DESCONECTAR;
            break;
        case PREFIJO_MOVIMIENTO:
            comando.tipo = MOVIMIENTO;
            uint8_t mov;
            skt.recvall(&mov, sizeof(mov));
            comando.movimiento = static_cast<enum Movimiento>(mov);
            break;
        case PREFIJO_DISPARAR:
            comando.tipo = DISPARO;
            uint16_t angulo_disparo;
            skt.recvall(&angulo_disparo, sizeof(angulo_disparo));
            comando.angulo = static_cast<float>(ntohs(angulo_disparo))/100;
            break;
        case PREFIJO_ROTACION:
            comando.tipo = ROTACION;
            uint16_t angulo;
            skt.recvall(&angulo, sizeof(angulo));
            comando.angulo = static_cast<float>(ntohs(angulo))/100;
            break;
        case PREFIJO_CAMBIO_ARMA:
            comando.tipo = CAMBIAR_ARMA;
            break;
        case PREFIJO_COMPRAR:
            comando.tipo = COMPRAR;
            uint8_t compra;
            skt.recvall(&compra, sizeof(compra));
            comando.compra = static_cast<enum Compra>(compra);
            break;
        case PREFIJO_SELECCIONAR_SKIN:
            comando.tipo = SELECCIONAR_SKIN;
            uint8_t skin;
            skt.recvall(&skin, sizeof(skin));
            comando.skin = static_cast<enum SkinTipos>(skin);
            break;
        case PREFIJO_BOMBA:
            comando.tipo = ACCION_SOBRE_BOMBA;
            uint8_t estado;
            skt.recvall(&estado, sizeof(estado));
            comando.estado_bomba = static_cast<enum EstadoBomba>(estado);
            break;
        case PREFIJO_DROPEAR:
            comando.tipo = DROPEAR;
            break;
        case PREFIJO_LEVANTAR:
            comando.tipo = LEVANTAR;
            break;
        default:
            break;
    }

    return true;
}

bool ServerProtocol::enviarID(int id_jugador) {
    uint16_t id = htons(static_cast<uint16_t>(id_jugador));
    skt.sendall(&id, sizeof(id));
    return true;
}

std::vector<std::string> ServerProtocol::recibir_inicio_juego() {
    uint8_t codigo;
    skt.recvall(&codigo, sizeof(codigo));
    std::vector<std::string> comando;
    if (codigo == PREFIJO_CREAR_PARTIDA) {
        comando.push_back("crear");
        uint16_t largo;
        skt.recvall(&largo, sizeof(largo));
        largo = ntohs(largo);
        std::vector<uint8_t> buffer(largo);
        skt.recvall(buffer.data(), largo);
        std::string username(buffer.begin(), buffer.end());
        comando.push_back(username);
    }else if (codigo == PREFIJO_UNIRSE_PARTIDA) {
        comando.push_back("unirse");
        uint16_t id_partida;
        skt.recvall(&id_partida, sizeof(id_partida));
        comando.push_back(std::to_string(ntohs(id_partida)));
        uint16_t largo_nombre;
        skt.recvall(&largo_nombre, sizeof(largo_nombre));
        largo_nombre = ntohs(largo_nombre);
        std::vector<uint8_t> buffer(largo_nombre);
        skt.recvall(buffer.data(), largo_nombre);
        std::string nombre(buffer.begin(), buffer.end());
        comando.push_back(nombre);
    } else if(codigo == PREFIJO_LISTAR) {
        comando.push_back("listar");
    }
    return comando;
}

void ServerProtocol::enviar_lista_partidas(const std::vector<std::string>& lista_partidas) {
    std::string mensaje = "Lista de partidas: \n";
    for (const std::string& info : lista_partidas) {
        mensaje += " - " + info + "\n";
    }
    enviar_mensaje(mensaje);
}

void ServerProtocol::enviar_mensaje(const std::string& mensaje) {
    std::vector<uint8_t> buffer;
    uint16_t largo = htons(static_cast<uint16_t>(mensaje.size()));
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&largo)[1]);
    buffer.insert(buffer.end(), mensaje.begin(), mensaje.end());
    skt.sendall(buffer.data(), buffer.size());
}

std::string ServerProtocol::recibir_path_mapa() {
    uint16_t largo;
    skt.recvall(&largo, sizeof(largo));
    largo = ntohs(largo);
    std::vector<uint8_t> buffer(largo);
    skt.recvall(buffer.data(), largo);
    std::string path_mapa(buffer.begin(), buffer.end());
    return path_mapa;
}

void ServerProtocol::enviar_mapa(const std::string& yaml_serializado){
    uint32_t tam = yaml_serializado.size();
    uint8_t header[4] = {
        static_cast<uint8_t>((tam >> 24) & 0xFF),
        static_cast<uint8_t>((tam >> 16) & 0xFF),
        static_cast<uint8_t>((tam >> 8) & 0xFF),
        static_cast<uint8_t>(tam & 0xFF)
    };
    skt.sendall(header, 4);
    skt.sendall((uint8_t*)yaml_serializado.data(), yaml_serializado.size());
}

void ServerProtocol::enviar_lista_mapas(const std::vector<std::pair<std::string, std::string>>& mapas) {
    uint8_t cantidad = mapas.size();
    skt.sendall(&cantidad, 1);  // Enviamos la cantidad de pares (mapa, imagen)

    for (const auto& par : mapas) {
        const std::string& nombre_mapa = par.first;
        const std::string& imagen_miniatura = par.second;

        // Enviar nombre del mapa
        uint16_t largo_nombre = nombre_mapa.size();
        uint16_t largo_nombre_red = htons(largo_nombre);
        skt.sendall(&largo_nombre_red, sizeof(largo_nombre_red));
        skt.sendall((uint8_t*)nombre_mapa.data(), largo_nombre);

        // Enviar nombre de la miniatura
        uint16_t largo_img = imagen_miniatura.size();
        uint16_t largo_img_red = htons(largo_img);
        skt.sendall(&largo_img_red, sizeof(largo_img_red));
        skt.sendall((uint8_t*)imagen_miniatura.data(), largo_img);
    }
}
